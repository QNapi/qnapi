/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#include "frmprogress.h"
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "libqnapi.h"
#include "qnapiapp.h"

frmProgress::frmProgress(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f) {
  qRegisterMetaType<SubtitleInfoList>("QNapiSubtitleInfoList");

  ui.setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose, false);
  setAttribute(Qt::WA_QuitOnClose, false);

  setBatchMode(false);

  connect(&getThread, SIGNAL(fileNameChange(const QString &)), ui.lbFileName,
          SLOT(setText(const QString &)));
  connect(&getThread, SIGNAL(actionChange(const QString &)), ui.lbAction,
          SLOT(setText(const QString &)));
  connect(&getThread, SIGNAL(progressChange(int, int, float)), this,
          SLOT(updateProgress(int, int, float)));
  connect(&getThread, SIGNAL(selectSubtitles(QString, SubtitleInfoList)), this,
          SLOT(selectSubtitles(QString, SubtitleInfoList)));
  connect(this, SIGNAL(subtitlesSelected(int)), &getThread,
          SLOT(subtitlesSelected(int)));
  connect(&getThread, SIGNAL(finished()), this, SLOT(downloadFinished()));

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
}

void frmProgress::receiveRequest(const QString &request) {
  enqueueFile(request);
  if (!getThread.isRunning()) download(LibQNapi::loadConfig());
  raise();
  activateWindow();
}

void frmProgress::enqueueFile(const QString &filePath) {
  static QMutex locker;
  locker.lock();
  if (QFile::exists(filePath)) {
    getThread.queue << filePath;
    updateProgress(-1, getThread.queue.size(), -1);
  }
  locker.unlock();
}

void frmProgress::enqueueFiles(const QStringList &fileList) {
  for (int i = 0; i < fileList.size(); i++) {
    enqueueFile(fileList.at(i));
  }
}

bool frmProgress::download(const QNapiConfig &config) {
  if (summary.isVisible()) {
    summary.close();
  }

  QNapi napi(config);

  // TODO: perform these checks earlier
  if (!napi.checkP7ZipPath()) {
    QMessageBox::warning(0, tr("Can not find p7zip!"),
                         tr("The path to the program p7zip is incorrect!"));
    return false;
  }

  if (!napi.checkTmpPath()) {
    QMessageBox::warning(
        0, tr("Invalid temporary directory!"),
        tr("Unable to write to the temporary directory! Check your settings."));
    return false;
  }

  if (getThread.queue.isEmpty()) {
    QMessageBox::warning(
        0, tr("No files!"),
        tr("Can't download subtitles as no movie files specified!"));
    return false;
  }

  if (!isVisible()) {
    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
    show();
  }

  showSummary = true;
  closeRequested = false;
  ui.pbCancel->setEnabled(true);

  getThread.setConfig(config);
  getThread.start();
  return true;
}

void frmProgress::updateProgress(int current, int all, float stageProgress) {
  static int lastCurrent, lastAll;
  static float lastStageProgress;

  static QMutex m;
  m.lock();

  if (current >= 0) lastCurrent = current;
  if (all >= 0) lastAll = all;
  if (stageProgress >= 0) lastStageProgress = stageProgress;

  QString windowTitle =
      (lastAll > 1) ? QString(tr("QNapi - downloading subtitles (%1/%2)"))
                          .arg(lastCurrent + 1)
                          .arg(lastAll)
                    : QString(tr("QNapi - downloading subtitles..."));
  setWindowTitle(windowTitle);

  ui.pbProgress->setMaximum(lastAll * 100);
  ui.pbProgress->setValue(lastCurrent * 100 + (int)(lastStageProgress * 100));

  m.unlock();
}

void frmProgress::selectSubtitles(QString fileName,
                                  SubtitleInfoList subtitles) {
  frmSelect.setFileName(fileName);
  frmSelect.setSubtitlesList(subtitles);

  int selIdx;

  if (frmSelect.exec() == QDialog::Accepted) {
    selIdx = frmSelect.getSelectedIndex();
  } else {
    selIdx = -1;
  }

  emit subtitlesSelected(selIdx);
}

void frmProgress::downloadFinished() {
  hide();

  QStringList queue = getThread.queue;
  getThread.queue.clear();

  mutex.lock();
  if (showSummary) {
    if (!getThread.criticalMessage.isEmpty()) {
      QMessageBox::critical(0, tr("Critical error!"),
                            getThread.criticalMessage);
    } else if (queue.size() > 0 && !getThread.subStatusList.isEmpty()) {
      summary.setSummaryList(getThread.subStatusList);
      summary.exec();
    }
  }

  if (closeRequested) close();

  mutex.unlock();
  if (batchMode) qApp->quit();
}

void frmProgress::closeEvent(QCloseEvent *event) {
  if (getThread.isRunning()) {
    if (QMessageBox::question(
            this, tr("QNapi"),
            tr("Do you want to cancel subtitles downloading?"),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
      mutex.lock();
      showSummary = false;
      getThread.requestAbort();
      ui.lbAction->setText(tr("Finishing the tasks..."));
      ui.lbFileName->setText("");
      ui.pbCancel->setEnabled(false);
      qApp->processEvents();
      closeRequested = true;
      mutex.unlock();
      event->ignore();
    } else {
      event->ignore();
    }
  } else {
    event->accept();
  }
}

void frmProgress::dragEnterEvent(QDragEnterEvent *event) {
  QUrl url(event->mimeData()->urls().at(0));
  QFileInfo fi(url.toLocalFile());
  if (fi.exists() && fi.isFile()) event->acceptProposedAction();
}

void frmProgress::dropEvent(QDropEvent *event) {
  QList<QUrl> urlList;
  QFileInfo info;

  urlList = event->mimeData()->urls();

  foreach (QUrl url, urlList) {
    info.setFile(url.toLocalFile());
    if (!info.exists() || !info.isFile()) continue;
    enqueueFile(url.toLocalFile());
  }
}

GetThread::GetThread() : config(LibQNapi::loadConfig()) {
  connect(this, SIGNAL(criticalError(const QString &)), this,
          SLOT(setCriticalMessage(const QString &)));
}

void GetThread::subtitlesSelected(int idx) {
  selIdx = idx;
  waitForDlg.unlock();
}

#define ABORT_POINT \
  {                 \
    if (abort) {    \
      return;       \
    }               \
  }

void GetThread::run() {
  abort = false;
  criticalMessage.clear();
  if (queue.size() <= 0) return;

  napiSuccess = napiFail = 0;
  subStatusList.clear();

  QNapi napi(config, specificEngine);

  emit progressChange(0, queue.size(), 0.0f);

  QString language = config.generalConfig().language();
  QString languageBackup = config.generalConfig().backupLanguage();

  for (int i = 0; i < queue.size(); i++) {
    ABORT_POINT

    QFileInfo fi(queue[i]);
    emit fileNameChange(fi.fileName());

    napi.setMoviePath(queue[i]);

    emit progressChange(i, queue.size(), 0.1f);
    emit actionChange(tr("Checking permissions of the video directory..."));

    if (!napi.checkWritePermissions()) {
      emit criticalError(tr("No permission to write to the directory '%1'!")
                             .arg(QFileInfo(queue[i]).path()));
      return;
    }

    napi.clearSubtitlesList();

    emit progressChange(i, queue.size(), 0.3f);
    emit actionChange(tr("Calculating checksum of the file..."));

    napi.checksum();

    ABORT_POINT

    bool found = false;
    SearchPolicy sp = config.generalConfig().searchPolicy();

    if (sp == SP_SEARCH_ALL_WITH_BACKUP_LANG) {
      foreach (QString e, napi.listLoadedEngines()) {
        emit progressChange(i, queue.size(), 0.4f);
        emit actionChange(
            tr("Searching for subtitles [%1] (%2)...").arg(language, e));
        found = napi.lookForSubtitles(language, e) || found;

        ABORT_POINT

        emit actionChange(
            tr("Searching for subtitles in alternative language [%1] (%2)...")
                .arg(languageBackup, e));
        found = napi.lookForSubtitles(languageBackup, e) || found;

        ABORT_POINT
      }
    } else {
      foreach (QString e, napi.listLoadedEngines()) {
        emit progressChange(i, queue.size(), 0.4f);
        emit actionChange(
            tr("Searching for subtitles [%1] (%2)...").arg(language, e));
        found = napi.lookForSubtitles(language, e) || found;

        if (sp == SP_BREAK_IF_FOUND && found) {
          break;
        }

        ABORT_POINT
      }

      if (!found && !languageBackup.isEmpty()) {
        foreach (QString e, napi.listLoadedEngines()) {
          emit progressChange(i, queue.size(), 0.45f);
          emit actionChange(
              tr("Searching for subtitles in alternative language [%1] (%2)...")
                  .arg(languageBackup, e));
          found = napi.lookForSubtitles(languageBackup, e) || found;

          if (sp == SP_BREAK_IF_FOUND && found) break;

          ABORT_POINT
        }
      }
    }

    if (!found) {
      ++napiFail;
      subStatusList << SubtitleInfo::fromFailed(queue[i]);
      continue;
    }

    // jesli mozna i potrzeba, listujemy dostepne napisy
    if (napi.needToShowList()) {
      emit selectSubtitles(QFileInfo(queue[i]).fileName(),
                           napi.listSubtitles());

      waitForDlg.lock();
      waitForDlg.lock();
      waitForDlg.unlock();
    } else {
      selIdx = napi.bestIdx();
    }

    ABORT_POINT

    if (selIdx == -1) {
      ++napiFail;
      subStatusList << SubtitleInfo::fromFailed(queue[i]);
      continue;
    }

    emit progressChange(i, queue.size(), 0.5);
    emit actionChange(tr("Downloading subtitles file..."));

    if (!napi.download(selIdx)) {
      ABORT_POINT

      ++napiFail;
      subStatusList << SubtitleInfo::fromFailed(queue[i]);
      continue;
    }

    ABORT_POINT

    emit progressChange(i, queue.size(), 0.65f);
    emit actionChange(tr("Unpacking subtitles file..."));

    if (!napi.unpack(selIdx)) {
      ++napiFail;
      subStatusList << SubtitleInfo::fromFailed(queue[i]);
      continue;
    }

    if (napi.ppEnabled()) {
      emit progressChange(i, queue.size(), 0.8f);
      emit actionChange(tr("Post-processing subtitles..."));
      napi.postProcessSubtitles();
    }

    emit progressChange(i, queue.size(), 0.9f);
    emit actionChange(tr("Adjusting subtitles..."));

    if (!napi.matchSubtitles()) {
      ABORT_POINT

      ++napiFail;
      subStatusList << SubtitleInfo::fromFailed(queue[i]);

      emit criticalError(tr("Could not adjust subtitles!"));
      return;
    }

    ++napiSuccess;
    subStatusList << napi.listSubtitles().at(selIdx);

    napi.cleanup();

    emit progressChange(i, queue.size(), 1.0f);
  }

  emit progressChange(queue.size() - 1, queue.size(), 1);
}

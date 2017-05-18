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

#include "frmscan.h"
#include "libqnapi.h"

frmScan::frmScan(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), scanConfig(LibQNapi::loadConfig().scanConfig()) {
  ui.setupUi(this);

  setAttribute(Qt::WA_QuitOnClose, false);

  connect(ui.pbDirectorySelect, SIGNAL(clicked()), this,
          SLOT(selectDirectory()));
  connect(ui.leDirectory, SIGNAL(textChanged(QString)), this,
          SLOT(leDirectoryTextChanged()));
  connect(ui.pbScan, SIGNAL(clicked()), this, SLOT(pbScanClicked()));
  connect(&scanThread, SIGNAL(addFile(QString)), this, SLOT(addFile(QString)));
  connect(&scanThread, SIGNAL(folderChange(QString)), this,
          SLOT(folderChange(QString)));
  connect(&scanThread, SIGNAL(scanFinished(bool)), this, SLOT(scanFinished()));
  connect(ui.pbSelectAll, SIGNAL(clicked()), this, SLOT(pbSelectAllClicked()));
  connect(ui.pbUnselectAll, SIGNAL(clicked()), this,
          SLOT(pbUnselectAllClicked()));
  connect(ui.pbInvertSelection, SIGNAL(clicked()), this,
          SLOT(pbInvertSelectionClicked()));
  connect(ui.lwMovies, SIGNAL(itemClicked(QListWidgetItem *)), this,
          SLOT(lwMoviesClicked(QListWidgetItem *)));

  if (QFileInfo(scanConfig.lastDir()).isDir())
    ui.leDirectory->setText(scanConfig.lastDir());

  QList<QString> scanFilters = scanConfig.filters();

  if (!scanFilters.isEmpty()) {
    ui.cbFilters->clear();

    for (int i = 0; i < scanFilters.size(); i++) {
      ui.cbFilters->addItem(scanFilters[i]);
    }
  }

  ui.leSkipFilters->setText(scanConfig.skipFilters());
  ui.cbSkipIfSubtitlesExists->setChecked(scanConfig.skipIfSubtitlesExist());

  iconFilm = QIcon(":/ui/film.png");

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
}

frmScan::~frmScan() {
  QList<QString> scanFilters;
  for (int i = 0; i < ui.cbFilters->count(); i++) {
    scanFilters << ui.cbFilters->itemText(i);
  }

  const ScanConfig updatedScanConfig =
      scanConfig.setLastDir(ui.leDirectory->text())
          .setFilters(scanFilters)
          .setSkipFilters(ui.leSkipFilters->text())
          .setSkipIfSubtitlesExist(ui.cbSkipIfSubtitlesExists->isChecked());

  const QNapiConfig config = LibQNapi::loadConfig();
  LibQNapi::writeConfig(config.setScanConfig(updatedScanConfig));
}

void frmScan::setInitDir(const QString &dir) {
  if (!dir.isEmpty() && QFileInfo(dir).isDir()) {
    ui.leDirectory->setText(QFileInfo(dir).absoluteFilePath());
  }
}

void frmScan::closeEvent(QCloseEvent *event) {
  if (scanThread.isRunning()) {
    scanThread.requestAbort();
    scanThread.wait();
  }

  event->accept();
}

void frmScan::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) close();
}

void frmScan::selectDirectory() {
  QString dir = QFileInfo(ui.leDirectory->text()).path();
  dir = QDir().exists(dir) ? dir : scanConfig.lastDir();

  QNapiOpenDialog openDialog(this, tr("Select the folder to scan"), dir,
                             QNapiOpenDialog::None);

  if (openDialog.selectDirectory()) {
    dir = openDialog.selectedFiles().first();

    if (QDir().exists(dir)) ui.leDirectory->setText(dir);
  }
}

void frmScan::leDirectoryTextChanged() {
  ui.pbScan->setEnabled(!ui.leDirectory->text().isEmpty()
                            ? QDir().exists(ui.leDirectory->text())
                            : false);
}

void frmScan::pbScanClicked() {
  if (!scanThread.isRunning()) {
    ui.leDirectory->setEnabled(false);
    ui.pbDirectorySelect->setEnabled(false);
    ui.pbScan->setText(tr("Cancel"));
    ui.lbAction->setText(tr("Scanning directories..."));
    ui.pbGet->setEnabled(false);
    enableControlWidgets(false);
    enableFilesWidgets(false);

    ui.lwMovies->clear();

    scanThread.setSearchPath(ui.leDirectory->text());
    scanThread.setFilters(ui.cbFilters->currentText());
    scanThread.setSkipFilters(ui.leSkipFilters->text());
    scanThread.setSkipIfSubtitlesExists(
        ui.cbSkipIfSubtitlesExists->isChecked());
    scanThread.setFollowSymLinks(ui.cbFollowSymLinks->isChecked());

    scanThread.start();
  } else {
    scanThread.requestAbort();
    ui.lbAction->setText(tr("Interrupting directory scanning..."));
    ui.pbScan->setEnabled(false);
    qApp->processEvents();
    scanThread.wait();
    ui.pbScan->setEnabled(true);
    scanFinished();
  }
}

void frmScan::addFile(const QString &fileName) {
  QListWidgetItem *item = new QListWidgetItem(
      iconFilm, QFileInfo(fileName).fileName(), ui.lwMovies);
  item->setToolTip(fileName);
  item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
  item->setCheckState(Qt::Unchecked);

  ui.lwMovies->addItem(item);
}

void frmScan::folderChange(const QString &dirName) {
  ui.lbAction->setText(tr("Scanning directory <b>%1</b>...").arg(dirName));
}

void frmScan::scanFinished() {
  enableControlWidgets(true);
  enableFilesWidgets(ui.lwMovies->count() > 0);
  ui.pbScan->setText(tr("Scan"));
  ui.lbAction->setText(
      (ui.lwMovies->count() > 0)
          ? tr("Select the videos you wish to download subtitles for.")
          : tr("No video files found."));
}

void frmScan::enableControlWidgets(bool enable) {
  ui.lbDirectory->setEnabled(enable);
  ui.leDirectory->setEnabled(enable);
  ui.pbDirectorySelect->setEnabled(enable);
  ui.lbFilter->setEnabled(enable);
  ui.cbFilters->setEnabled(enable);
  ui.lbSkip->setEnabled(enable);
  ui.leSkipFilters->setEnabled(enable);
  ui.cbSkipIfSubtitlesExists->setEnabled(enable);
  ui.cbFollowSymLinks->setEnabled(enable);
}

void frmScan::enableFilesWidgets(bool enable) {
  ui.lbMovies->setEnabled(enable);
  ui.lwMovies->setEnabled(enable);
  ui.pbSelectAll->setEnabled(enable);
  ui.pbUnselectAll->setEnabled(enable);
  ui.pbInvertSelection->setEnabled(enable);
}

void frmScan::lwMoviesClicked(QListWidgetItem *item) {
  Q_UNUSED(item);
  checkPbGetEnabled();
}

void frmScan::pbSelectAllClicked() {
  ui.lwMovies->selectAll();

  checkPbGetEnabled();
}

void frmScan::pbUnselectAllClicked() {
  ui.lwMovies->unselectAll();

  checkPbGetEnabled();
}

void frmScan::pbInvertSelectionClicked() {
  ui.lwMovies->invertSelection();

  checkPbGetEnabled();
}

void frmScan::checkPbGetEnabled() {
  for (int i = 0; i < ui.lwMovies->count(); ++i) {
    if (ui.lwMovies->item(i)->checkState() == Qt::Checked) {
      ui.pbGet->setEnabled(true);
      return;
    }
  }

  ui.pbGet->setEnabled(false);
}

void frmScan::accept() {
  selectedFiles.clear();

  for (int i = 0; i < ui.lwMovies->count(); i++) {
    if (ui.lwMovies->item(i)->checkState() == Qt::Checked)
      selectedFiles << ui.lwMovies->item(i)->toolTip();
  }

  QDialog::accept();
}

ScanFilesThread::ScanFilesThread() : staticConfig(LibQNapi::staticConfig()) {}

void ScanFilesThread::run() {
  abort = false;
  fileList.clear();
  visited.clear();

  QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot |
                          QDir::Readable | QDir::Hidden;

  if (!followSymLinks) filters |= QDir::NoSymLinks;

  emit scanFinished(doScan(searchPath, filters));
}

bool ScanFilesThread::doScan(const QString &path, QDir::Filters filters) {
  QString myPath = QFileInfo(path).absoluteFilePath();

  if (!QDir().exists(myPath)) return false;

  QString myCPath = QDir(path).canonicalPath();

  if (visited.contains(myCPath)) return true;

  visited << myCPath;

  emit folderChange(myPath);

  QFileInfoList list = QDir(myPath).entryInfoList(scanFilters, filters);

  for (QFileInfoList::iterator p = list.begin(); p != list.end(); p++) {
    if (abort) return false;

    if ((*p).isDir() && ((*p).absoluteFilePath() != myPath)) {
      if (!doScan((*p).absoluteFilePath(), filters)) return false;
    } else {
      if (!QFile::exists((*p).absoluteFilePath())) continue;

      bool subtitleFileFound = false;
      if (skipIfSubtitlesExists) {
        foreach (QString subExt, staticConfig->subtitleExtensions()) {
          if (QFile::exists((*p).absolutePath() + "/" +
                            (*p).completeBaseName() + "." + subExt)) {
            subtitleFileFound = true;
            break;
          }
        }
      }

      if (subtitleFileFound) {
        continue;
      }

      bool skip = false;
      for (QStringList::iterator s = skipFilters.begin();
           s != skipFilters.end(); s++) {
        if ((*s).isEmpty()) continue;
        if ((*p).fileName().indexOf(*s) >= 0) {
          skip = true;
          break;
        }
      }

      if (skip) continue;

      fileList << (*p).absoluteFilePath();
      emit addFile((*p).absoluteFilePath());
    }
  }

  return true;
}

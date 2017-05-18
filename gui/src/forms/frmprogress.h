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

#ifndef __FRMPROGRESS__H__
#define __FRMPROGRESS__H__

#include <QMutex>

#include "ui_frmprogress.h"

#include "frmsummary.h"

#include "config/qnapiconfig.h"

#include <Maybe.h>
#include "frmlistsubtitles.h"
#include "qnapi.h"
#include "qnapiopendialog.h"
#include "qnapithread.h"
#include "subtitleinfo.h"

class GetThread : public QNapiThread {
  Q_OBJECT

 public:
  GetThread();

 signals:
  void fileNameChange(const QString &newfileName);
  void actionChange(const QString &newAction);
  void progressChange(int current, int all, float stageProgress);
  void criticalError(const QString &message);
  void selectSubtitles(QString fileName, SubtitleInfoList subtitles);

 private slots:
  void setCriticalMessage(const QString &msg) { criticalMessage = msg; }
  void subtitlesSelected(int idx);

 public:
  void setSpecificEngine(Maybe<QString> engine) { specificEngine = engine; }

  void setLanguages(QString language, QString languageBackup,
                    bool languageBackupPassed) {
    lang = language;
    langBackup = languageBackup;
    langBackupPassed = languageBackupPassed;
  }
  void setConfig(const QNapiConfig &configuration) { config = configuration; }

  void run();

  QStringList queue;
  Maybe<QString> specificEngine;
  QList<SubtitleInfo> subStatusList;
  QString lang, langBackup;
  bool langBackupPassed;
  int napiSuccess, napiFail;
  QString criticalMessage;
  QMutex waitForDlg;
  int selIdx;
  QNapiConfig config;
};

class frmProgress : public QWidget {
  Q_OBJECT

 public:
  frmProgress(QWidget *parent = 0, Qt::WindowFlags f = 0);

  void clearSpecificEngine() { getThread.setSpecificEngine(nothing()); }
  void setSpecificEngine(QString engine) {
    getThread.setSpecificEngine(just(engine));
  }

  void setBatchMode(bool value) { batchMode = value; }
  void setBatchLanguages(QString lang, QString langBackup,
                         bool langBackupPassed) {
    getThread.setLanguages(lang, langBackup, langBackupPassed);
  }
  void setTargetFormatOverride(QString value) { targetFormatOverride = value; }
  void setTargetExtOverride(QString value) { targetExtOverride = value; }
  bool isBatchMode() { return batchMode; }

 signals:
  void subtitlesSelected(int idx);

 public slots:
  void receiveRequest(const QString &request);
  void enqueueFile(const QString &file);
  void enqueueFiles(const QStringList &fileList);
  bool download();
  void updateProgress(int current, int all, float stageProgress);
  void selectSubtitles(QString fileName, SubtitleInfoList subtitles);
  void downloadFinished();

 private:
  void closeEvent(QCloseEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);

  Ui::frmProgress ui;
  GetThread getThread;
  frmListSubtitles frmSelect;
  frmSummary summary;
  QString targetFormatOverride, targetExtOverride;

  bool batchMode, showSummary, closeRequested;
  QMutex mutex;
};

#endif

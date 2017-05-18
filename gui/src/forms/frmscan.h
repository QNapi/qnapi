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

#ifndef __FRMSCAN__H__
#define __FRMSCAN__H__

#include "ui_frmscan.h"

#include "config/scanconfig.h"
#include "config/staticconfig.h"

#include "qnapiopendialog.h"
#include "qnapithread.h"

#include "frmsummary.h"

#include <QSet>
#include <QSharedPointer>
#include <QtWidgets>

class ScanFilesThread : public QNapiThread {
  Q_OBJECT
 public:
  ScanFilesThread();
  void run();
  void setSearchPath(const QString &path) { searchPath = path; }
  void setFilters(const QString &filters) { scanFilters = filters.split(" "); }
  void setSkipFilters(const QString &filters) {
    skipFilters = filters.split(" ");
  }
  void setSkipIfSubtitlesExists(bool skip) { skipIfSubtitlesExists = skip; }
  void setFollowSymLinks(bool follow) { followSymLinks = follow; }

  QStringList fileList;

 signals:
  void addFile(const QString &fileName);
  void scanFinished(bool result);
  void folderChange(const QString &folder);

 private:
  bool doScan(const QString &path, QDir::Filters filters);

  QSharedPointer<const StaticConfig> staticConfig;
  QString searchPath;
  QStringList scanFilters, skipFilters;
  bool skipIfSubtitlesExists, followSymLinks;
  QSet<QString> visited;
};

class frmScan : public QDialog {
  Q_OBJECT

 public:
  frmScan(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~frmScan();

  void setInitDir(const QString &dir);

  QStringList getSelectedFiles() { return selectedFiles; }

 public slots:

  void accept();

 private:
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

  Ui::frmScan ui;
  const ScanConfig scanConfig;
  ScanFilesThread scanThread;

  QIcon iconFilm;
  QStringList selectedFiles;

 private slots:

  void selectDirectory();
  void leDirectoryTextChanged();
  void pbScanClicked();
  void addFile(const QString &fileName);
  void folderChange(const QString &dirName);
  void scanFinished();
  void enableControlWidgets(bool enable);
  void enableFilesWidgets(bool enable);
  void lwMoviesClicked(QListWidgetItem *item);
  void pbSelectAllClicked();
  void pbUnselectAllClicked();
  void pbInvertSelectionClicked();
  void checkPbGetEnabled();
};

#endif

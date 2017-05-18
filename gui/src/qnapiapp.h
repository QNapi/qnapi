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

#ifndef __QNAPIAPP__H__
#define __QNAPIAPP__H__

#include "engines/subtitledownloadenginesregistry.h"

#include "qnapiopendialog.h"

#include "forms/frmabout.h"
#include "forms/frmconvert.h"
#include "forms/frmoptions.h"
#include "forms/frmprogress.h"
#include "forms/frmscan.h"
#include "forms/frmsummary.h"

#include "qcumber/qsingleapplication.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QEvent>
#include <QFileOpenEvent>
#include <QMenu>
#include <QSharedPointer>
#include <QString>
#include <QSystemTrayIcon>

class QNapiApp : public QSingleApplication {
  Q_OBJECT
 public:
  QNapiApp(int &argc, char **argv, bool useGui, const QString &appName);
  ~QNapiApp();

  void createTrayIcon();
  void showTrayMessage(QString title, QString msg);

  frmProgress *progress();

 public slots:

  bool showOpenDialog(QString engine = "");
  bool showScanDialog(QString init_dir = "");
  void showConvertDialog();
  void showCreateAccount(const QString &engineName) const;
  void showOSUploadDialog() const;
  void showSettings();
  void showAbout();
  void tryQuit();

 signals:
  void downloadFile(const QString &fileName);

 private:
  bool event(QEvent *ev);

  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu, *napiSubMenu, *osSubMenu, *napisy24SubMenu;
  QAction *getAction, *scanAction, *convertAction, *napiGetAction,
      *napiCreateUserAction, *osGetAction, *osAddAction, *osCreateUserAction,
      *napisy24GetAction, *napisy24CreateUserAction, *settingsAction,
      *aboutAction, *quitAction;

  QNapiOpenDialog *openDialog;

  frmProgress *f_progress;
  frmOptions *f_options;
  frmAbout *f_about;
  frmScan *f_scan;
  frmConvert *f_convert;

  QDateTime creationDT;

  QSharedPointer<const SubtitleDownloadEnginesRegistry> enginesRegistry;

 private slots:

  void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif

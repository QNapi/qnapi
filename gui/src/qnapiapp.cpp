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

#include "qnapiapp.h"

#include <iostream>
#include "libqnapi.h"

QNapiApp::QNapiApp(int &argc, char **argv, const QString &appName)
    : QSingleApplication(argc, argv, appName),
      creationDT(QDateTime::currentDateTime()),
      enginesRegistry(LibQNapi::subtitleDownloadEngineRegistry()) {
  setAttribute(Qt::AA_UseHighDpiPixmaps, true);

  openDialog = 0;

  f_progress = 0;
  f_options = 0;
  f_about = 0;
  f_scan = 0;
  f_convert = 0;

  getAction = 0;
  scanAction = 0;
  convertAction = 0;
  napiGetAction = 0;
  napiCreateUserAction = 0;
  osGetAction = 0;
  osAddAction = 0;
  settingsAction = 0;
  aboutAction = 0;
  quitAction = 0;

  napiSubMenu = 0;
  osSubMenu = 0;
  napisy24SubMenu = 0;
  trayIconMenu = 0;
  trayIcon = 0;
}

QNapiApp::~QNapiApp() {
  if (openDialog) delete openDialog;

  if (f_progress) delete f_progress;
  if (f_options) delete f_options;
  if (f_about) delete f_about;
  if (f_scan) delete f_scan;
  if (f_convert) delete f_convert;

  if (getAction) delete getAction;
  if (scanAction) delete scanAction;
  if (convertAction) delete convertAction;
  if (napiGetAction) delete napiGetAction;
  if (napiCreateUserAction) delete napiCreateUserAction;
  if (osGetAction) delete osGetAction;
  if (osAddAction) delete osAddAction;
  if (settingsAction) delete settingsAction;
  if (aboutAction) delete aboutAction;
  if (quitAction) delete quitAction;

  if (napiSubMenu) delete napiSubMenu;
  if (osSubMenu) delete osSubMenu;
  if (napisy24SubMenu) delete napisy24SubMenu;
  if (trayIconMenu) delete trayIconMenu;
  if (trayIcon) delete trayIcon;
}

frmProgress *QNapiApp::progress() {
  if (!f_progress) {
    f_progress = new frmProgress();
    if (!f_progress) abort();
    connect(this, SIGNAL(request(QString)), f_progress,
            SLOT(receiveRequest(QString)));
    connect(this, SIGNAL(downloadFile(const QString &)), f_progress,
            SLOT(receiveRequest(const QString &)));
  }
  return f_progress;
}

void QNapiApp::createTrayIcon() {
  getAction = new QAction(tr("Download subtitles"), 0);
  connect(getAction, &QAction::triggered,
          [this] { showOpenDialog(LibQNapi::loadConfig()); });

  scanAction = new QAction(tr("Scan directories"), 0);
  connect(scanAction, &QAction::triggered,
          [this] { showScanDialog(LibQNapi::loadConfig()); });

  convertAction = new QAction(tr("Convert subtitles"), 0);
  connect(convertAction, SIGNAL(triggered()), this, SLOT(showConvertDialog()));

  napiGetAction = new QAction(tr("Download subtitles"), 0);
  connect(napiGetAction, &QAction::triggered,
          [this] { showOpenDialog(LibQNapi::loadConfig(), "NapiProjekt"); });

  napiCreateUserAction = new QAction(tr("Create an account"), 0);
  connect(napiCreateUserAction, &QAction::triggered,
          [this] { showCreateAccount("NapiProjekt"); });

  osGetAction = new QAction(tr("Download subtitles"), 0);
  connect(osGetAction, &QAction::triggered,
          [this] { showOpenDialog(LibQNapi::loadConfig(), "OpenSubtitles"); });

  osAddAction = new QAction(tr("Upload subtitles"), 0);
  connect(osAddAction, SIGNAL(triggered()), this, SLOT(showOSUploadDialog()));

  osCreateUserAction = new QAction(tr("Create an account"), 0);
  connect(osCreateUserAction, &QAction::triggered,
          [this] { showCreateAccount("OpenSubtitles"); });

  napisy24GetAction = new QAction(tr("Download subtitles"), 0);
  connect(napisy24GetAction, &QAction::triggered,
          [this] { showOpenDialog(LibQNapi::loadConfig(), "Napisy24"); });

  napisy24CreateUserAction = new QAction(tr("Create an account"), 0);
  connect(napisy24CreateUserAction, &QAction::triggered,
          [this] { showCreateAccount("OpenSNapisy24ubtitles"); });

  settingsAction = new QAction(tr("Settings"), 0);
  connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

  aboutAction = new QAction(tr("About"), 0);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

  quitAction = new QAction(tr("Quit"), 0);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(tryQuit()));

  napiSubMenu = new QMenu(0);
  napiSubMenu->setTitle(tr("NapiProjekt"));
  napiSubMenu->addAction(napiGetAction);
  napiSubMenu->addAction(napiCreateUserAction);

  osSubMenu = new QMenu(0);
  osSubMenu->setTitle(tr("OpenSubtitles"));
  osSubMenu->addAction(osGetAction);
  osSubMenu->addAction(osAddAction);
  osSubMenu->addAction(osCreateUserAction);

  napisy24SubMenu = new QMenu(0);
  napisy24SubMenu->setTitle(tr("Napisy24"));
  napisy24SubMenu->addAction(napisy24GetAction);
  napisy24SubMenu->addAction(napisy24CreateUserAction);

  trayIconMenu = new QMenu(0);
  trayIconMenu->addAction(getAction);
  trayIconMenu->addAction(scanAction);
  trayIconMenu->addAction(convertAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addMenu(napiSubMenu);
  trayIconMenu->addMenu(osSubMenu);
  trayIconMenu->addMenu(napisy24SubMenu);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(settingsAction);
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(0);
  trayIcon->setContextMenu(trayIconMenu);

  trayIcon->setIcon(QIcon::fromTheme("qnapi-panel", QIcon(":/icon/qnapi.png")));

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
          SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

  trayIcon->show();
}

void QNapiApp::showTrayMessage(QString title, QString msg) {
  trayIcon->showMessage(title, msg);
}

bool QNapiApp::showOpenDialog(const QNapiConfig &config, QString engine) {
  QStringList fileList;

  if (!openDialog) {
    QString initDir = config.lastOpenedDir();

    if (!QFileInfo(initDir).isDir()) {
      initDir = QDir::homePath();
    }

    openDialog = new QNapiOpenDialog(
        0, tr("Select one or more video files to download subtitles for"),
        initDir, QNapiOpenDialog::Movies);
  } else if (openDialog) {
    openDialog->raise();
    return true;
  } else if (f_progress) {
    f_progress->raise();
    return true;
  }

  if (openDialog->selectFiles()) {
    fileList = openDialog->selectedFiles();

    if (!fileList.isEmpty()) {
      QString dialogPath = QFileInfo(fileList.first()).absolutePath();
      LibQNapi::writeConfig(config.setLastOpenedDir(dialogPath));
    }
  }

  delete openDialog;
  openDialog = 0;

  if (!fileList.isEmpty()) {
    if (!engine.isEmpty()) {
      progress()->setSpecificEngine(engine);
    } else {
      progress()->clearSpecificEngine();
    }

    progress()->enqueueFiles(fileList);
    progress()->download(config);
  } else if (progress()->isBatchMode()) {
    return false;
  }

  return true;
}

bool QNapiApp::showScanDialog(const QNapiConfig &config, QString init_dir) {
  if (!f_scan) f_scan = new frmScan();

  if (f_scan->isVisible()) {
    f_scan->raise();
    return false;
  }

  f_scan->setInitDir(init_dir);

  bool result = false;

  if (f_scan->exec() == QDialog::Accepted) {
    result = true;
    progress()->enqueueFiles(f_scan->getSelectedFiles());
    progress()->download(config);
  }

  delete f_scan;
  f_scan = 0;
  return result;
}

void QNapiApp::showConvertDialog() {
  if (!f_convert) f_convert = new frmConvert();

  if (f_convert->isVisible()) {
    f_convert->raise();
    return;
  }

  f_convert->exec();

  delete f_convert;
  f_convert = 0;
}

void QNapiApp::showCreateAccount(const QString &engineName) const {
  Maybe<QUrl> maybeRegistrationUrl =
      enginesRegistry->engineMetadata(engineName).registrationUrl();

  if (maybeRegistrationUrl) {
    QDesktopServices::openUrl(maybeRegistrationUrl.value());
  }
}

void QNapiApp::showOSUploadDialog() const {
  QDesktopServices::openUrl(QUrl("http://www.opensubtitles.org/upload"));
}

void QNapiApp::showSettings() {
  if (!f_options) {
    f_options = new frmOptions();
    f_options->readConfig(LibQNapi::loadConfig());
  }

  if (f_options->isVisible()) {
    f_options->raise();
    return;
  }

  if (f_options->exec() == QDialog::Accepted) f_options->writeConfig();

  delete f_options;
  f_options = 0;
}

void QNapiApp::showAbout() {
  if (!f_about) f_about = new frmAbout();
  if (f_about->isVisible()) {
    f_about->raise();
    return;
  }
  f_about->exec();
  delete f_about;
  f_about = 0;
}

void QNapiApp::tryQuit() {
  if (progress()->isVisible() && !progress()->close()) return;
  quit();
}

void QNapiApp::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
  if (reason == QSystemTrayIcon::Trigger)
#ifndef Q_OS_MAC
    showOpenDialog(LibQNapi::loadConfig())
#endif
        ;
}

bool QNapiApp::event(QEvent *ev) {
  if (ev->type() == QEvent::FileOpen) {
    bool batchMode = creationDT.secsTo(QDateTime::currentDateTime()) <= 1;
    progress()->setBatchMode(batchMode);
    emit downloadFile(static_cast<QFileOpenEvent *>(ev)->file());
  } else {
    return QApplication::event(ev);
  }

  return true;
}

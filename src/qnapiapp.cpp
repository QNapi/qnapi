/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

QNapiApp::QNapiApp(int & argc, char **argv, bool useGui, const QString & appName)
    : QSingleApplication(argc, argv, useGui, appName), creationDT(QDateTime::currentDateTime())
{
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
    trayIconMenu = 0;
    trayIcon = 0;

    f_main = 0;
}

QNapiApp::~QNapiApp()
{
    if(openDialog) delete openDialog;

    if(f_progress) delete f_progress;
    if(f_options) delete f_options;
    if(f_about) delete f_about;
    if(f_scan) delete f_scan;
    if(f_convert) delete f_convert;

    if(getAction) delete getAction;
    if(scanAction) delete scanAction;
    if(convertAction) delete convertAction;
    if(napiGetAction) delete napiGetAction;
    if(napiCreateUserAction) delete napiCreateUserAction;
    if(osGetAction) delete osGetAction;
    if(osAddAction) delete osAddAction;
    if(settingsAction) delete settingsAction;
    if(aboutAction) delete aboutAction;
    if(quitAction) delete quitAction;

    if(napiSubMenu) delete napiSubMenu;
    if(trayIconMenu) delete trayIconMenu;
    if(trayIcon) delete trayIcon;

    if(f_main) delete f_main;
}

frmProgress * QNapiApp::progress()
{
    if(!f_progress)
    {
        f_progress = new frmProgress();
        if(!f_progress) abort();
        connect(this, SIGNAL(downloadFile(const QString &)),
                f_progress, SLOT(receiveRequest(const QString &)));
    }
    return f_progress;
}

void QNapiApp::createTrayIcon()
{
    getAction = new QAction(tr("Pobierz napisy"), 0);
    connect(getAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

    scanAction = new QAction(tr("Skanuj katalogi"), 0);
    connect(scanAction, SIGNAL(triggered()), this, SLOT(showScanDialog()));

    convertAction = new QAction(tr("Konwertuj napisy"), 0);
    connect(convertAction, SIGNAL(triggered()), this, SLOT(showConvertDialog()));

    napiGetAction = new QAction(tr("Pobierz napisy"), 0);
    connect(napiGetAction, SIGNAL(triggered()), this, SLOT(showNPGetDialog()));

    napiCreateUserAction = new QAction(tr("Załóż konto"), 0);
    connect(napiCreateUserAction, SIGNAL(triggered()), this, SLOT(showNPCreateUser()));

    osGetAction = new QAction(tr("Pobierz napisy"), 0);
    connect(osGetAction, SIGNAL(triggered()), this, SLOT(showOSGetDialog()));

    osAddAction = new QAction(tr("Dodaj napisy"), 0);
    connect(osAddAction, SIGNAL(triggered()), this, SLOT(showOSUploadDialog()));

    osCreateUserAction = new QAction(tr("Załóż konto"), 0);
    connect(osCreateUserAction, SIGNAL(triggered()), this, SLOT(showOSCreateUser()));

    napisy24GetAction = new QAction(tr("Pobierz napisy"), 0);
    connect(napisy24GetAction, SIGNAL(triggered()), this, SLOT(showNapisy24GetDialog()));

    napisy24CreateUserAction = new QAction(tr("Załóż konto"), 0);
    connect(napisy24CreateUserAction, SIGNAL(triggered()), this, SLOT(showNapisy24CreateUser()));

    settingsAction = new QAction(tr("Opcje"), 0);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    aboutAction = new QAction(tr("O programie"), 0);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    quitAction = new QAction(tr("Zakończ"), 0);
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

    trayIcon->setIcon(QIcon(":/icon/qnapi.png"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
}

void QNapiApp::createMainWindow()
{
    if(!f_main) {
        f_main = new frmMain();

        progress(); // initialize to be able to connect signals

        connect(f_main, SIGNAL(droppedFiles(QStringList)), f_progress, SLOT(receiveRequests(QStringList)));
        connect(f_main, SIGNAL(droppedDirectory(QString)), this, SLOT(showScanDialog(QString)));

        connect(f_main, SIGNAL(settings()), this, SLOT(showSettings()));
        connect(f_main, SIGNAL(about()), this, SLOT(showAbout()));
        // TODO: help?
        connect(f_main, SIGNAL(quit()), this, SLOT(tryQuit()));

        connect(f_main, SIGNAL(download()), this, SLOT(showOpenDialog()));
        connect(f_main, SIGNAL(scan()), this, SLOT(showScanDialog()));
        connect(f_main, SIGNAL(convert()), this, SLOT(showConvertDialog()));
    }

    f_main->show();
}

bool QNapiApp::showOpenDialog(QString engine)
{
    QStringList fileList;

    if(!openDialog)
    {
        openDialog = new QNapiOpenDialog(0,
                            tr("Wybierz jeden lub więcej plików z filmami"),
                            GlobalConfig().previousDialogPath(),
                            QNapiOpenDialog::Movies);
    }
    else if(openDialog)
    {
        openDialog->raise();
        return true;
    }
    else if(f_progress)
    {
        f_progress->raise();
        return true;
    }

    if(openDialog->selectFiles())
    {
        fileList = openDialog->selectedFiles();

        if(!fileList.isEmpty())
        {
            QString dialogPath = QFileInfo(fileList[0]).absolutePath();
            GlobalConfig().setPreviousDialogPath(dialogPath);
            GlobalConfig().save();
        }
    }

    delete openDialog;
    openDialog = 0;

    if(!fileList.isEmpty())
    {
        if(!engine.isEmpty())
        {
            QStringList e;
            e << engine;
            progress()->setEngines(e);
        }

        progress()->enqueueFiles(fileList);
        progress()->download();
    }
    else if(progress()->isBatchMode())
    {
        return false;
    }

    return true;
}

bool QNapiApp::showScanDialog(QString init_dir)
{
    if(!f_scan) f_scan = new frmScan();

    if(f_scan->isVisible())
    {
        f_scan->raise();
        return false;
    }

    f_scan->setInitDir(init_dir);

    bool result = false;

    if(f_scan->exec() == QDialog::Accepted)
    {
        result = true;
        progress()->enqueueFiles(f_scan->getSelectedFiles());
        progress()->download();
    }

    delete f_scan;
    f_scan = 0;
    return result;
}

void QNapiApp::showConvertDialog()
{
    if(!f_convert) f_convert = new frmConvert();

    if(f_convert->isVisible())
    {
        f_convert->raise();
        return;
    }

    f_convert->exec();

    delete f_scan;
    f_scan = 0;
}

void QNapiApp::showNPGetDialog()
{
    showOpenDialog("NapiProjekt");
}

void QNapiApp::showNPCreateUser()
{
    QDesktopServices::openUrl(QUrl("http://www.napiprojekt.pl/rejestracja"));
}

void QNapiApp::showOSGetDialog()
{
    showOpenDialog("OpenSubtitles");
}

void QNapiApp::showOSUploadDialog()
{
    QDesktopServices::openUrl(QUrl("http://www.opensubtitles.org/upload"));
}

void QNapiApp::showOSCreateUser()
{
    QDesktopServices::openUrl(QUrl("http://www.opensubtitles.org/newuser"));
}

void QNapiApp::showNapisy24GetDialog()
{
    showOpenDialog("Napisy24");
}

void QNapiApp::showNapisy24CreateUser()
{
    QDesktopServices::openUrl(QUrl("http://napisy24.pl/cb-registration/registers"));
}

void QNapiApp::showSettings()
{
    if(!f_options)
    {
        f_options = new frmOptions();
        f_options->readConfig();
    }

    if(f_options->isVisible())
    {
        f_options->raise();
        return;
    }

    if(f_options->exec() == QDialog::Accepted)
        f_options->writeConfig();

    delete f_options;
    f_options = 0;
}

void QNapiApp::showAbout()
{
    if(!f_about) f_about = new frmAbout();
    if(f_about->isVisible())
    {
        f_about->raise();
        return;
    }
    f_about->exec();
    delete f_about;
    f_about = 0;
}

void QNapiApp::tryQuit()
{
    if(progress()->isVisible() && !progress()->close()) return;
    quit();
}

void QNapiApp::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
#ifndef Q_OS_MAC
        showOpenDialog()
#endif
    ;
}

bool QNapiApp::event(QEvent *ev)
{
    if(ev->type() == QEvent::FileOpen) {
        bool batchMode = creationDT.secsTo(QDateTime::currentDateTime()) <= 1;
        progress()->setBatchMode(batchMode);
        emit downloadFile(static_cast<QFileOpenEvent*>(ev)->file());
    } else {
        return QApplication::event(ev);
    }

    return true;
}

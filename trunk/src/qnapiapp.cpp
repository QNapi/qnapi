/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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

QNapiApp::QNapiApp(int & argc, char **argv, bool useGui) : QSingleApplication(argc, argv, useGui)
{
	openDialog = 0;

	f_progress = 0;
	f_options = 0;
	f_createUser = 0;
	f_about = 0;
	f_scan = 0;
	f_upload = 0;
	f_correct = 0;
	f_report = 0;

	getAction = 0;
	scanAction = 0;
	napiGetAction = 0;
	napiAddAction = 0;
	napiCorrectAction = 0;
	napiReportAction = 0;
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
}

QNapiApp::~QNapiApp()
{
	if(openDialog) delete openDialog;

	if(f_progress) delete f_progress;
	if(f_options) delete f_options;
	if(f_createUser) delete f_createUser;
	if(f_about) delete f_about;
	if(f_scan) delete f_scan;
	if(f_upload) delete f_upload;
	if(f_correct) delete f_correct;
	if(f_report) delete f_report;

	if(getAction) delete getAction;
	if(scanAction) delete scanAction;
	if(napiGetAction) delete napiGetAction;
	if(napiAddAction) delete napiAddAction;
	if(napiCorrectAction) delete napiCorrectAction;
	if(napiReportAction) delete napiReportAction;
	if(napiCreateUserAction) delete napiCreateUserAction;
	if(osGetAction) delete osGetAction;
	if(osAddAction) delete osAddAction;
	if(settingsAction) delete settingsAction;
	if(aboutAction) delete aboutAction;
	if(quitAction) delete quitAction;

	if(napiSubMenu) delete napiSubMenu;
	if(trayIconMenu) delete trayIconMenu;
	if(trayIcon) delete trayIcon;
}

frmProgress * QNapiApp::progress()
{
	if(!f_progress)
	{
		f_progress = new frmProgress();
		if(!f_progress) abort();
		connect(this, SIGNAL(request(QString)),
				f_progress, SLOT(receiveRequest(QString)));
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

	napiGetAction = new QAction(tr("Pobierz napisy"), 0);
//	connect(napiGetAction, SIGNAL(triggered()), this, SLOT(showUploadDialog()));

	napiAddAction = new QAction(tr("Dodaj napisy"), 0);
	connect(napiAddAction, SIGNAL(triggered()), this, SLOT(showUploadDialog()));

	napiCorrectAction = new QAction(tr("Popraw napisy"), 0);
	connect(napiCorrectAction, SIGNAL(triggered()), this, SLOT(showCorrectDialog()));

	napiReportAction = new QAction(tr("Zgłoś niepasujące"), 0);
	connect(napiReportAction, SIGNAL(triggered()), this, SLOT(showReportDialog()));

	napiCreateUserAction = new QAction(tr("Załóż konto"), 0);
	connect(napiCreateUserAction, SIGNAL(triggered()), this, SLOT(showCreateUser()));

	osGetAction = new QAction(tr("Pobierz napisy"), 0);
//	connect(osGetAction, SIGNAL(triggered()), this, SLOT(showOSGetDialog()));

	osAddAction = new QAction(tr("Dodaj napisy"), 0);
//	connect(osAddAction, SIGNAL(triggered()), this, SLOT(showOSUploadDialog()));

	settingsAction = new QAction(tr("Opcje"), 0);
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

	aboutAction = new QAction(tr("O programie"), 0);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

	quitAction = new QAction(tr("Zakończ"), 0);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(tryQuit()));

	napiSubMenu = new QMenu(0);
	napiSubMenu->setTitle(tr("napiprojekt.pl"));
	napiSubMenu->addAction(napiGetAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(napiAddAction);
	napiSubMenu->addAction(napiCorrectAction);
	napiSubMenu->addAction(napiReportAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(napiCreateUserAction);

	osSubMenu = new QMenu(0);
	osSubMenu->setTitle(tr("opensubtitles.org"));
	osSubMenu->addAction(osGetAction);
	osSubMenu->addAction(osAddAction);

	trayIconMenu = new QMenu(0);
	trayIconMenu->addAction(getAction);
	trayIconMenu->addAction(scanAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addMenu(napiSubMenu);
	trayIconMenu->addMenu(osSubMenu);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(settingsAction);
	trayIconMenu->addAction(aboutAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(0);
	trayIcon->setContextMenu(trayIconMenu);

#ifdef Q_WS_MAC
	trayIcon->setIcon(QIcon(":/icon/qnapi-22-mono.png"));
#else
	trayIcon->setIcon(QIcon(":/icon/qnapi.png"));
#endif

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->show();
}

void QNapiApp::showTrayMessage(QString title, QString msg)
{
	trayIcon->showMessage(title, msg);
}

bool QNapiApp::showOpenDialog()
{
	QStringList fileList;

	if(!openDialog)
	{
		if( !(openDialog = new QNapiOpenDialog(0,
								tr("Wybierz jeden lub więcej plików z filmami"),
								GlobalConfig().previousDialogPath(),
								QNapiOpenDialog::Movies))
		)
		{
			return false;
		}
	}

	if(openDialog->selectFiles())
	{
		fileList = openDialog->selectedFiles();
		GlobalConfig().setPreviousDialogPath(openDialog->directory().path());
	}

	delete openDialog;
	openDialog = 0;

	if(!fileList.isEmpty())
	{
		progress()->enqueueFiles(fileList);
		progress()->download();
	}
	else if(progress()->isBatchMode())
	{
		return false;
	}

	return true;
}

void QNapiApp::showScanDialog()
{
	if(!f_scan) f_scan = new frmScan();
	if(f_scan->isVisible())
	{
		f_scan->raise();
		return;
	}
	f_scan->exec();
	delete f_scan;
	f_scan = 0;
}

void QNapiApp::showUploadDialog()
{
	if(!f_upload) f_upload = new frmUpload();
	if(f_upload->isVisible())
	{
		f_upload->raise();
		return;
	}
	f_upload->exec();
	delete f_upload;
	f_upload = 0;
}

void QNapiApp::showCorrectDialog()
{
	if(!f_correct) f_correct = new frmCorrect();
	if(f_correct->isVisible())
	{
		f_correct->raise();
		return;
	}
	f_correct->exec();
	delete f_correct;
	f_correct = 0;
}

void QNapiApp::showReportDialog()
{
	if(!f_report) f_report = new frmReport();
	if(f_report->isVisible())
	{
		f_report->raise();
		return;
	}
	f_report->exec();
	delete f_report;
	f_report = 0;
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

void QNapiApp::showCreateUser()
{
	if(!f_createUser) f_createUser = new frmCreateUser();
	if(f_createUser->isVisible())
	{
		f_createUser->raise();
		return;
	}
	f_createUser->exec();
	delete f_createUser;
	f_createUser = 0;
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
#ifndef Q_WS_MAC
		showOpenDialog()
#endif
	;
}

bool QNapiApp::event(QEvent *ev)
{
	if(ev->type() == QEvent::FileOpen)
		emit downloadFile(static_cast<QFileOpenEvent*>(ev)->file());
	else
		return QApplication::event(ev);

	return true;
}

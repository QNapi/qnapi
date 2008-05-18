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

QNapiApp::QNapiApp(int argc, char **argv) : QSingleApplication(argc, argv)
{
	openDialog = 0;
	progress = 0;
	options = 0;
	createUser = 0;
	about = 0;
	scan = 0;
	upload = 0;
	correct = 0;
	report = 0;

	progress = new frmProgress();
	progress->connect(this, SIGNAL(request(QString)), SLOT(receiveRequest(QString)));
}

QNapiApp::~QNapiApp()
{
	if(progress) delete progress;
}

void QNapiApp::createTrayIcon()
{
	getAction = new QAction(tr("Pobierz"), 0);
	connect(getAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

	scanAction = new QAction(tr("Skanuj katalogi i dopasuj napisy"), 0);
	connect(scanAction, SIGNAL(triggered()), this, SLOT(showScanDialog()));

	addNewAction = new QAction(tr("Dodaj nowe"), 0);
	connect(addNewAction, SIGNAL(triggered()), this, SLOT(showUploadDialog()));

	addCorrectedAction = new QAction(tr("Dodaj poprawione"), 0);
	connect(addCorrectedAction, SIGNAL(triggered()), this, SLOT(showCorrectDialog()));

	reportBadAction = new QAction(tr("Zgłoś niepasujące"), 0);
	connect(reportBadAction, SIGNAL(triggered()), this, SLOT(showReportDialog()));

	optionsAction = new QAction(tr("Opcje"), 0);
	connect(optionsAction, SIGNAL(triggered()), this, SLOT(showOptions()));

	createUserAction = new QAction(tr("Załóż konto"), 0);
	connect(createUserAction, SIGNAL(triggered()), this, SLOT(showCreateUser()));

	aboutAction = new QAction(tr("O programie"), 0);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

	quitAction = new QAction(tr("Zakończ"), 0);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(tryQuit()));

	napiSubMenu = new QMenu(0);
	napiSubMenu->setTitle(tr("Napisy"));
	napiSubMenu->addAction(getAction);
	napiSubMenu->addAction(scanAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(addNewAction);
	napiSubMenu->addAction(addCorrectedAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(reportBadAction);

	trayIconMenu = new QMenu(0);
	trayIconMenu->addMenu(napiSubMenu);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(optionsAction);
	trayIconMenu->addAction(createUserAction);
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

void QNapiApp::showOpenDialog()
{
	QStringList fileList;

	if(!openDialog)
	{
		if( !(openDialog = new QNapiOpenDialog(0,
								tr("Wybierz jeden lub więcej plików z filmami"),
								GlobalConfig().previousDialogPath(),
								QNapiOpenDialog::Movies))
		)
			return;
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
		progress->enqueueFiles(fileList);
		progress->download();
	}
	else if(progress->isBatchMode())
		quit();
}

void QNapiApp::showScanDialog()
{
	if(!scan) scan = new frmScan();
	if(scan->isVisible())
	{
		scan->raise();
		return;
	}
	scan->exec();
	delete scan;
	scan = 0;
}

void QNapiApp::showUploadDialog()
{
	if(!upload) upload = new frmUpload();
	if(upload->isVisible())
	{
		upload->raise();
		return;
	}
	upload->exec();
	delete upload;
	upload = 0;
}

void QNapiApp::showCorrectDialog()
{
	if(!correct) correct = new frmCorrect();
	if(correct->isVisible())
	{
		correct->raise();
		return;
	}
	correct->exec();
	delete correct;
	correct = 0;
}

void QNapiApp::showReportDialog()
{
	if(!report) report = new frmReport();
	if(report->isVisible())
	{
		report->raise();
		return;
	}
	report->exec();
	delete report;
	report = 0;
}

void QNapiApp::showOptions()
{
	if(!options)
	{
		options = new frmOptions();
		options->readConfig();
	}

	if(options->isVisible())
	{
		options->raise();
		return;
	}

	if(options->exec() == QDialog::Accepted)
		options->writeConfig();

	delete options;
	options = 0;
}

void QNapiApp::showCreateUser()
{
	if(!createUser) createUser = new frmCreateUser();
	if(createUser->isVisible())
	{
		createUser->raise();
		return;
	}
	createUser->exec();
	delete createUser;
	createUser = 0;
}

void QNapiApp::showAbout()
{
	if(!about) about = new frmAbout();
	if(about->isVisible())
	{
		about->raise();
		return;
	}
	about->exec();
	delete about;
	about = 0;
}


void QNapiApp::tryQuit()
{
	if(progress->isVisible() && !progress->close()) return;
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

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

#ifndef __FRMPROGRESS__H__
#define __FRMPROGRESS__H__

#include "ui_frmprogress.h"
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMenu>
#include <QCloseEvent>
#include <QDesktopWidget>

#include "frmabout.h"
#include "frmoptions.h"
#include "frmupload.h"
#include "frmcorrect.h"
#include "frmreport.h"
#include "frmscan.h"
#include "frmcreateuser.h"
#include "frmsummary.h"

#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiprojektengine.h"
#include "qnapiopendialog.h"

class GetThread : public QNapiThread
{
	Q_OBJECT

	signals:
		void fileNameChange(const QString & newfileName);
		void actionChange(const QString & newAction);
		void progressChange(int current, int all, float stageProgress);
		void criticalError(const QString & message);

	private slots:
		void setCriticalMessage(const QString & msg)
		{
			criticalMessage = msg;
		}

	public:
		GetThread()
		{
			connect(this, SIGNAL(criticalError(const QString &)),
					this, SLOT(setCriticalMessage(const QString &)));
		}
		void run();

		QStringList queue, gotList, failedList;
		int napiSuccess, napiFail;
		QString criticalMessage;
};

class frmProgress: public QWidget
{
	Q_OBJECT

	public:
		frmProgress(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmProgress(){};

		void createTrayIcon();

	public slots:

		void receiveRequest(QString request);
		void enqueueFile(const QString &file);
		void enqueueFiles(const QStringList &fileList);
		bool download();
		void updateProgress(int current, int all, float stageProgress);
		void downloadFinished();

		void showOpenDialog();
		void showScanDialog();
		void showUploadDialog();
		void showCorrectDialog();
		void showReportDialog();
		void showOptions();
		void showCreateUser();
		void showAbout();
		void quit();

		void setBatchMode(bool value) { batchMode = value; }
		bool isBatchMode() { return batchMode; }

	private:
		void closeEvent(QCloseEvent *event);
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);

		Ui::frmProgress ui;

		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu, *napiSubMenu;
		QAction *getAction, *scanAction, *addNewAction, *addCorrectedAction,
				*reportBadAction, *optionsAction, *createUserAction,
				*aboutAction, *quitAction;

		QNapiOpenDialog *openDialog;
		frmOptions *options;
		frmCreateUser *createUser;
		frmAbout *about;
		frmScan *scan;
		frmUpload *upload;
		frmCorrect *correct;
		frmReport *report;

		GetThread getThread;

		bool batchMode;

		bool showSummary;
		QMutex mutex;

	private slots:
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif

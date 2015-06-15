/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

#include <QApplication>
#include <QString>
#include <QEvent>
#include <QFileOpenEvent>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QDesktopServices>

#include "qcumber/qsingleapplication.h"

#include "forms/frmprogress.h"
#include "forms/frmabout.h"
#include "forms/frmoptions.h"
#include "forms/frmupload.h"
#include "forms/frmcorrect.h"
#include "forms/frmreport.h"
#include "forms/frmscan.h"
#include "forms/frmcreateuser.h"
#include "forms/frmsummary.h"

#include "qnapiconfig.h"
#include "qnapiopendialog.h"


class QNapiApp : public QSingleApplication
{
	Q_OBJECT
	public:
		QNapiApp(int & argc, char **argv, bool useGui, const QString & appName);
		~QNapiApp();

		void createTrayIcon();
		void showTrayMessage(QString title, QString msg);

		frmProgress * progress();

	public slots:

		bool showOpenDialog(QString engine = "");
		bool showScanDialog(QString init_dir = "");

		void showNPGetDialog();
		void showNPUploadDialog();
		void showNPCorrectDialog();
		void showNPReportDialog();
		void showNPCreateUser();

		void showOSGetDialog();
		void showOSUploadDialog();
		void showOSCreateUser();
		
        void showNapisy24GetDialog();
        void showNapisy24CreateUser();

		void showSettings();
		void showAbout();
		void tryQuit();

	signals:
		void downloadFile(const QString & fileName);

	private:

		bool event(QEvent *ev);

		QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu, *napiSubMenu, *osSubMenu, *napisy24SubMenu;
		QAction *getAction, *scanAction, *napiGetAction, *napiAddAction,
				*napiCorrectAction, *napiReportAction, *napiCreateUserAction,
				*osGetAction, *osAddAction, *osCreateUserAction,
                *napisy24GetAction, *napisy24CreateUserAction,
                *settingsAction, *aboutAction, *quitAction;

		QNapiOpenDialog *openDialog;
		
		frmProgress *f_progress;
		frmOptions *f_options;
		frmCreateUser *f_createUser;
		frmAbout *f_about;
		frmScan *f_scan;
		frmUpload *f_upload;
		frmCorrect *f_correct;
		frmReport *f_report;

	private slots:

		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

};

#endif

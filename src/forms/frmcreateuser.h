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

#ifndef __FRMCREATEUSER__H__
#define __FRMCREATEUSER__H__

#include "ui_frmcreateuser.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QRegExp>

//#include "napi.h"
#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiprojektengine.h"

class CreateUserThread : public QNapiThread
{
Q_OBJECT
	public:
		void run();
		void setUserParams(const QString & u_nick, const QString & u_pass, const QString & u_mail)
		{
			setTerminationEnabled(true);
			nick = u_nick;
			pass = u_pass;
			mail = u_mail;
		}

		QNapiProjektEngine::ReportResult taskResult;

	signals:
		void creatingFinished(bool result);
		void serverMessage(QString msg);

	private:
		QString nick, pass, mail;
};

class frmCreateUser: public QDialog
{
Q_OBJECT
	public:
		frmCreateUser(QWidget *parent = 0, Qt::WindowFlags f = 0);
		~frmCreateUser() {};

	private:
		void closeEvent(QCloseEvent *event);
		bool validEmail(const QString & email);

		Ui::frmCreateUser ui;

		CreateUserThread createUserThread;

	private slots:
		void checkCreateEnable();
		void pbCreateClicked();
		void creatingFinished(bool result = false);
		void serverMessage(QString msg);
};

#endif

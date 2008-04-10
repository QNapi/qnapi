/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef __FRMCREATEUSER__H__
#define __FRMCREATEUSER__H__

#include "ui_frmcreateuser.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QRegExp>

#include "napi.h"
#include "napithread.h"
#include "qnapiconfig.h"

class CreateUserThread : public NapiThread
{
Q_OBJECT
	public:
		void run();
		void setUserParams(const QString & u_nick, const QString & u_pass, const QString & u_mail)
		{
			nick = u_nick;
			pass = u_pass;
			mail = u_mail;
		}

		napiReportResult taskResult;

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
		frmCreateUser(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmCreateUser() {};

	private:
		void closeEvent(QCloseEvent *event);
		bool validEmail(const QString & email);

		Ui::frmCreateUser ui;

		CreateUserThread createUserThread;

	private slots:
		void checkCreateEnable();
		void pbCreateClicked();
		void pbCancelClicked();
		void creatingFinished(bool result);
		void serverMessage(QString msg);
};

#endif

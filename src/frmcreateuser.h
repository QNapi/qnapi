/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */
#ifndef __FRMCREATEUSER__H__
#define __FRMCREATEUSER__H__

#include "ui_frmcreateuser.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QDesktopWidget>

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

		Ui::frmCreateUser ui;

		CreateUserThread createUserThread;

	private slots:
		void resizeEvent(QResizeEvent *event);

		void checkCreateEnable();
		void pbCreateClicked();
		void pbCancelClicked();
		void creatingFinished(bool result);
		void serverMessage(QString msg);
};

#endif

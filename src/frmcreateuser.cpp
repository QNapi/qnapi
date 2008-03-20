/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */
#include "frmcreateuser.h"

frmCreateUser::frmCreateUser(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_QuitOnClose, false);
	
	connect(ui.pbCreate, SIGNAL(clicked()), this, SLOT(pbCreateClicked()));
	connect(ui.pbCancel, SIGNAL(clicked()), this, SLOT(pbCancelClicked()));
	
	connect(ui.leLogin, SIGNAL(textChanged(QString)), this, SLOT(checkCreateEnable()));
	connect(ui.lePass, SIGNAL(textChanged(QString)), this, SLOT(checkCreateEnable()));
	connect(ui.leRepeatPass, SIGNAL(textChanged(QString)), this, SLOT(checkCreateEnable()));
	connect(ui.leMail, SIGNAL(textChanged(QString)), this, SLOT(checkCreateEnable()));
	connect(&createThread, SIGNAL(creatingFinished(bool)), this, SLOT(creatingFinished(bool)));
	connect(&createThread, SIGNAL(serverMessage(QString)), this, SLOT(serverMessage(QString)));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmCreateUser::closeEvent(QCloseEvent *event)
{
	if(createThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać zakładanie konta?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			ui.pbCreate->setEnabled(false);
			qApp->processEvents();
			createThread.terminate();
			createThread.wait();
		}
		else
		{
			event->ignore();
			return;
		}
	}
	
	event->accept();
}

void frmCreateUser::resizeEvent(QResizeEvent *resize)
{
	int b = 5;
	ui.gridLayout->setGeometry(QRect(b, b, resize->size().width() - b*2,
								resize->size().height() - b*2));
}

void frmCreateUser::checkCreateEnable()
{
	bool e = true;
	e &= !ui.leLogin->text().isEmpty();
	e &= !ui.lePass->text().isEmpty();
	e &= !ui.leRepeatPass->text().isEmpty();
	e &= !ui.leMail->text().isEmpty();
	e &= (ui.lePass->text() == ui.leRepeatPass->text());
	ui.pbCreate->setEnabled(e);
	
	if(ui.lePass->text() != ui.leRepeatPass->text())
		ui.lbStatus->setText(tr("Wpisane hasła różnią się od siebie!"));
	else
		ui.lbStatus->setText(tr("Wpisz dane potrzebne do założenia konta"));
	if(e)
		ui.lbStatus->setText(tr("Teraz możesz założyć konto na serwerze NAPI-PROJEKT"));
}

void frmCreateUser::pbCreateClicked()
{
	
}

void frmCreateUser::pbCancelClicked()
{
	if(createThread.isRunning())
		pbCreateClicked();
	else
		close();
}

void frmCreateUser::creatingFinished(bool result)
{
	
}

void frmCreateUser::serverMessage(QString msg)
{
	
}

void CreateThread::run()
{
	QString *response = new QString();
	bool r;
	if(r = napiCreateUser(nick, pass, mail, response))
		emit serverMessage(*response);

	delete response;
	emit creatingFinished(r);
}

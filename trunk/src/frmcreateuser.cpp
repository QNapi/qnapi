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
	connect(&createUserThread, SIGNAL(creatingFinished(bool)), this, SLOT(creatingFinished(bool)));
	connect(&createUserThread, SIGNAL(serverMessage(QString)), this, SLOT(serverMessage(QString)));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmCreateUser::closeEvent(QCloseEvent *event)
{
	if(createUserThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać zakładanie konta?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			ui.pbCreate->setEnabled(false);
			qApp->processEvents();
			createUserThread.terminate();
			createUserThread.wait();
		}
		else
		{
			event->ignore();
			return;
		}
	}

	event->accept();
}

bool frmCreateUser::validEmail(const QString & email)
{
	return QRegExp("^(.+)@(.+)\\.(.+)$").exactMatch(email);
}

void frmCreateUser::checkCreateEnable()
{
	bool e = true;
	e &= !ui.leLogin->text().isEmpty();
	e &= !ui.lePass->text().isEmpty();
	e &= !ui.leRepeatPass->text().isEmpty();
	e &= !ui.leMail->text().isEmpty();
	e &= validEmail(ui.leMail->text());
	e &= (ui.lePass->text().length() >= 5);
	e &= (ui.lePass->text() == ui.leRepeatPass->text());
	ui.pbCreate->setEnabled(e);

	if (!ui.lePass->text().isEmpty() && (ui.lePass->text().length() < 5))
		ui.lbStatus->setText(tr("Hasło musi mieć conajmniej 5 znaków!"));
	else if(ui.lePass->text() != ui.leRepeatPass->text())
		ui.lbStatus->setText(tr("Wpisane hasła różnią się od siebie!"));
	else if(!ui.leMail->text().isEmpty() && !validEmail(ui.leMail->text()))
		ui.lbStatus->setText(tr("Wpisz poprawny adres e-mail."));
	else 
		ui.lbStatus->setText(tr("Wpisz dane potrzebne do założenia konta"));
	if(e)
		ui.lbStatus->setText(tr("Teraz możesz założyć konto na serwerze NAPI-PROJEKT"));
}

void frmCreateUser::pbCreateClicked()
{
	if(!createUserThread.isRunning())
	{
		ui.pbCreate->setText(tr("Przerwij"));
		ui.lbStatus->setText(tr("Zakładanie konta na serwerze NAPI..."));
		ui.leLogin->setEnabled(false);
		ui.lePass->setEnabled(false);
		ui.leRepeatPass->setEnabled(false);
		ui.leMail->setEnabled(false);

		createUserThread.setUserParams(ui.leLogin->text(), ui.lePass->text(), ui.leMail->text());
		createUserThread.start();
	}
	else
	{
		ui.pbCreate->setEnabled(false);
		ui.lbStatus->setText(tr("Anulowanie operacji..."));
		qApp->processEvents();
		createUserThread.terminate();
		createUserThread.wait();

		creatingFinished(false);
		ui.pbCreate->setEnabled(true);
		ui.lbStatus->setText(tr("Przerwano zakładanie konta"));
	}
}

void frmCreateUser::pbCancelClicked()
{
	if(createUserThread.isRunning())
		pbCreateClicked();
	else
		close();
}

void frmCreateUser::creatingFinished(bool result)
{
	ui.lbStatus->setText(result
							? tr("Operacja zakończona")
							: tr("Zakładanie konta nie powiodło się"));
	ui.pbCreate->setText(tr("Załóż konto"));
	ui.leLogin->setEnabled(true);
	ui.lePass->setEnabled(true);
	ui.leRepeatPass->setEnabled(true);
	ui.leMail->setEnabled(true);
}

void frmCreateUser::serverMessage(QString msg)
{
	QRegExp extract("<A HREF=\"index.php3\\?msg=(.*)\">", Qt::CaseInsensitive);
	if(extract.indexIn(msg) > -1)
		msg = extract.cap(1).replace("%20", " ");

	if((msg.indexOf("NPc0") == 0) || (msg.indexOf("Konto zostalo zalozone!") == 0))
	{
		msg = tr("Konto na serwerze NAPI zostało założone!");
		QMessageBox::information(this, tr("Konto założone"), msg);

		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz uaktualnić ustawienia w programie?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			GlobalConfig().setNick(ui.leLogin->text());
			GlobalConfig().setPass(ui.lePass->text());
			GlobalConfig().save();
		}

		if(createUserThread.isRunning())
			createUserThread.wait();
		close();
	}
	else
	{
		msg = tr("Odpowiedź serwera: ") + tr(qPrintable(msg));
		QMessageBox::information(this, tr("Operacja zakończona"), msg);
	}
}

void CreateUserThread::run()
{
	QString *response = new QString();
	bool r;
	if((r = QNapiProjektEngine::createUser(nick, pass, mail, response)))
		emit serverMessage(*response);

	delete response;
	emit creatingFinished(r);
}

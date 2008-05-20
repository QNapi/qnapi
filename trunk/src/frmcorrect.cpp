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

#include "frmcorrect.h"

frmCorrect::frmCorrect(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_QuitOnClose, false);
	
	connect(ui.pbMovieSelect, SIGNAL(clicked()), this, SLOT(selectMovie()));
	connect(ui.pbSubtitlesSelect, SIGNAL(clicked()), this, SLOT(selectSubtitles()));
	connect(ui.leMovieSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
	connect(ui.leSubtitlesSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
	connect(ui.teComment, SIGNAL(textChanged()), this, SLOT(checkPostEnable()));
	connect(ui.pbPost, SIGNAL(clicked()), this, SLOT(pbPostClicked()));
	connect(&postThread, SIGNAL(postFinished(bool)), this, SLOT(postFinished(bool)));
	connect(&postThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmCorrect::closeEvent(QCloseEvent *event)
{
	if(postThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać wysyłanie poprawki?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			pbPostClicked();
		}
		event->ignore();
	}
	else
		event->accept();
}

void frmCorrect::selectMovie()
{
	QString fileName;
	QNapiOpenDialog openDialog(this, tr("Wskaż plik z filmem"),
								GlobalConfig().previousDialogPath(), QNapiOpenDialog::Movies);

	if(openDialog.selectFile())
	{
		fileName = openDialog.selectedFiles().first();
		GlobalConfig().setPreviousDialogPath(openDialog.directory().path());
	}

	if(!fileName.isEmpty() && QFile::exists(fileName))
		ui.leMovieSelect->setText(fileName);

	QFileInfo fi(fileName);
	QString propSubtitleFile = fi.path() + "/" + fi.completeBaseName() + ".txt";

	if(QFile::exists(propSubtitleFile))
		ui.leSubtitlesSelect->setText(propSubtitleFile);
}

void frmCorrect::selectSubtitles()
{
	QString fileName;
	QNapiOpenDialog openDialog(this, tr("Wskaż plik z napisami"),
								GlobalConfig().previousDialogPath(), QNapiOpenDialog::Subtitles);

	if(openDialog.selectFile())
	{
		fileName = openDialog.selectedFiles().first();
		GlobalConfig().setPreviousDialogPath(openDialog.directory().path());
	}

	if(!fileName.isEmpty() && QFile::exists(fileName))
		ui.leSubtitlesSelect->setText(fileName);
}

void frmCorrect::checkPostEnable()
{
	ui.pbPost->setEnabled(
			QFile::exists(ui.leMovieSelect->text()) &&
			QFile::exists(ui.leSubtitlesSelect->text()) &&
			!ui.teComment->toPlainText().isEmpty()
		);
	ui.lbAction->setText(ui.pbPost->isEnabled()
							? tr("Teraz możesz wysłać poprawione napisy.")
							: tr("Wypełnij wszystkie pola, aby wysłać poprawkę"));
}

void frmCorrect::pbPostClicked()
{
	if(!postThread.isRunning())
	{
		ui.leMovieSelect->setEnabled(false);
		ui.pbMovieSelect->setEnabled(false);
		ui.leSubtitlesSelect->setEnabled(false);
		ui.pbSubtitlesSelect->setEnabled(false);
		ui.teComment->setEnabled(false);
		ui.pbPost->setText(tr("Zatrzymaj"));
		ui.lbAction->setText(tr("Wysyłanie poprawki do serwera..."));
		
		postThread.setPostParams(ui.leMovieSelect->text(),
									ui.leSubtitlesSelect->text(),
									ui.teComment->toPlainText(),
									(ui.cbLanguage->currentIndex() == 0) ? "PL" : "ENG");
		postThread.start();
	}
	else
	{
		postThread.requestAbort();
		ui.lbAction->setText(tr("Przerywanie wysyłania..."));
		ui.pbPost->setEnabled(false);
		qApp->processEvents();
	}
}

void frmCorrect::postFinished(bool interrupted)
{
	ui.leMovieSelect->setEnabled(true);
	ui.pbMovieSelect->setEnabled(true);
	ui.leSubtitlesSelect->setEnabled(true);
	ui.pbSubtitlesSelect->setEnabled(true);
	ui.teComment->setEnabled(true);
	ui.pbPost->setEnabled(true);
	ui.pbPost->setText(tr("Wyślij"));

	if(interrupted)
	{
		ui.lbAction->setText(tr("Przerwano wysyłanie poprawki."));
	}
	else
	{
		switch(postThread.taskResult)
		{
			case QNapiProjektEngine::NAPI_FAIL: ui.lbAction->setText(tr("Błąd podczas wysyłania poprawki.")); break;
			default: ui.lbAction->setText(tr("Poprawka wysłana."));
		}
	}
}

void frmCorrect::invalidUserPass()
{
	QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void PostThread::run()
{
	abort = false;

	if(!QNapiProjektEngine::checkUser(GlobalConfig().nick(), GlobalConfig().pass()))
	{
		emit invalidUserPass();
		emit postFinished();
		return;
	}

	if(abort)
	{
		postFinished(true);
		return;
	}

	QNapiProjektEngine *napi;

	if((napi = new QNapiProjektEngine(movie, subtitles)))
	{
		taskResult = napi->uploadSubtitles(language, GlobalConfig().nick(),
											GlobalConfig().pass(), true, comment);
		delete napi;
	}

	postFinished();
}

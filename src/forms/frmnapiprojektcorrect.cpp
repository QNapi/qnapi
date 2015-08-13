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

#include "frmnapiprojektcorrect.h"

frmNapiProjektCorrect::frmNapiProjektCorrect(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);
    
    connect(ui.pbMovieSelect, SIGNAL(clicked()), this, SLOT(selectMovie()));
    connect(ui.pbSubtitlesSelect, SIGNAL(clicked()), this, SLOT(selectSubtitles()));
    connect(ui.leMovieSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
    connect(ui.leSubtitlesSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
    connect(ui.teComment, SIGNAL(textChanged()), this, SLOT(checkPostEnable()));
    connect(ui.pbPost, SIGNAL(clicked()), this, SLOT(pbPostClicked()));
    connect(&postThread, SIGNAL(postFinished(bool)), this, SLOT(postFinished(bool)));
    connect(&postThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

void frmNapiProjektCorrect::closeEvent(QCloseEvent *event)
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

void frmNapiProjektCorrect::selectMovie()
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

void frmNapiProjektCorrect::selectSubtitles()
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

void frmNapiProjektCorrect::checkPostEnable()
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

void frmNapiProjektCorrect::pbPostClicked()
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

void frmNapiProjektCorrect::postFinished(bool interrupted)
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

void frmNapiProjektCorrect::invalidUserPass()
{
    QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void PostThread::run()
{
    abort = false;

    if(!QNapiProjektEngine::checkUser(  GlobalConfig().nick("NapiProjekt"),
                                        GlobalConfig().pass("NapiProjekt")))
    {
        emit invalidUserPass();
        emit postFinished(true);
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
        taskResult = napi->uploadSubtitles( language,
                                            GlobalConfig().nick("NapiProjekt"),
                                            GlobalConfig().pass("NapiProjekt"),
                                            true,
                                            comment);
        delete napi;
    }

    postFinished();
}

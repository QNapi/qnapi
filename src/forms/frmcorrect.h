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

#ifndef __FRMCORRECT__H__
#define __FRMCORRECT__H__

#include "ui_frmcorrect.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>

#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiprojektengine.h"
#include "qnapiopendialog.h"

class PostThread : public QNapiThread
{
    Q_OBJECT
    public:
        void run();
        void setPostParams(const QString & movie_file, const QString & subtitles_file,
                            const QString & comment_txt, const QString & lang)
        {
            movie = movie_file;
            subtitles = subtitles_file;
            comment = comment_txt;
            language = lang;
        }

        QNapiProjektEngine::UploadResult taskResult;

    signals:
        void postFinished(bool interrupted = false);
        void invalidUserPass();

    private:
        QString movie, subtitles, comment, language;
};

class frmCorrect: public QDialog
{
Q_OBJECT
    public:
        frmCorrect(QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~frmCorrect() {};

    private:
        void closeEvent(QCloseEvent *event);

        Ui::frmCorrect ui;

        PostThread postThread;

    private slots:
        void selectMovie();
        void selectSubtitles();
        void checkPostEnable();
        void pbPostClicked();
        void postFinished(bool interrupt = false);
        void invalidUserPass();
};

#endif

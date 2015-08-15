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

#ifndef __FRMUPLOAD__H__
#define __FRMUPLOAD__H__

#include "ui_frmnapiprojektupload.h"
#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QSet>

#include "qnapithread.h"
#include "qnapiconfig.h"
#include "engines/qnapiprojektengine.h"
#include "qnapiopendialog.h"

class ScanThread : public QNapiThread
{
    Q_OBJECT
    public:
        void run();
        void setSearchPath(const QString & path) { searchPath = path; }
        void setFollowSymLinks(bool follow) { followSymLinks = follow; }

        QStringList fileList;
        unsigned int folders, movies, subtitles;

    signals:
        void scanFinished(bool result);
        void folderChange(const QString & folder);

    private:
        bool doScan(const QString & path, QDir::Filters filters);
        QString searchPath;
        QStringList searchFilters;
        bool followSymLinks;
        QSet<QString> visited;
};

class UploadThread : public QNapiThread
{
    Q_OBJECT
    public:
        void run();
        void setMovieList(const QStringList & list) { movieList = list; }
        unsigned int added_new, added_ok, failed;
        bool invalidUP;

    signals:
        void progressChange(int progress);
        void fileNameChange(const QString & filename);
        void checkingUserPass();
        void invalidUserPass();

    private:
        QStringList movieList;
};

class frmNapiProjektUpload: public QDialog
{
    Q_OBJECT
    public:
        frmNapiProjektUpload(QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~frmNapiProjektUpload() {};

    private:
        void closeEvent(QCloseEvent *event);
        
        Ui::frmUpload ui;
        
        ScanThread scanThread;
        UploadThread uploadThread;

    private slots:
        void selectDirectory();
        void leSelectDirectoryChanged();
        void pbScanClicked();
        void folderChange(const QString & folder);
        void scanFinished(bool result);
        void pbUploadClicked();
        void fileNameChange(const QString & filename);
        void checkingUserPass();
        void uploadFinished();
        void invalidUserPass();
};

#endif

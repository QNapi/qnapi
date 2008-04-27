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

#ifndef __FRMUPLOAD__H__
#define __FRMUPLOAD__H__

#include "ui_frmupload.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>

//#include "napi.h"
#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiprojektengine.h"

class ScanThread : public QNapiThread
{
	Q_OBJECT
	public:
		void run();
		void setSearchPath(const QString & path) { searchPath = path; }

		QStringList fileList;
		unsigned int folders, movies, subtitles;

	signals:
		void scanFinished(bool result);
		void folderChange(const QString & folder);

	private:
		bool doScan(const QString & path);
		QString searchPath;
		QStringList searchFilters;
};

class UploadThread : public QNapiThread
{
	Q_OBJECT
	public:
		void run();
		void setMovieList(const QStringList & list) { movieList = list; }
		unsigned int added_new, added_ok, failed;

	signals:
		void uploadFinished(bool interrupt = false);
		void progressChange(int progress);
		void fileNameChange(const QString & filename);
		void checkingUserPass();
		void invalidUserPass();

	private:
		QStringList movieList;
};

class frmUpload: public QDialog
{
	Q_OBJECT
	public:
		frmUpload(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmUpload() {};

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
		void uploadFinished(bool interrupt = false);
		void invalidUserPass();
};

#endif

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

#ifndef __FRMSCAN__H__
#define __FRMSCAN__H__

#include "ui_frmscan.h"
#include <QDesktopWidget>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
#include <QVariant>

#include "napithread.h"
#include "napi.h"
#include "qnapiconfig.h"

#include "frmsummary.h"

class ScanFilesThread : public NapiThread
{
	Q_OBJECT
	public:
		void run();
		void setSearchPath(const QString & path) { searchPath = path; }
		void setFilters(const QString & filters) { scanFilters = filters.split(" "); }
		void setSkipFilters(const QString & filters) { skipFilters = filters.split(" "); }
		void setSkipIfSubtitlesExists(bool skip) { skipIfSubtitlesExists = skip; }

		QStringList fileList;

	signals:
		void addFile(const QString & fileName);
		void scanFinished(bool result);
		void folderChange(const QString & folder);

	private:
		bool doScan(const QString & path);
		QString searchPath;
		QStringList scanFilters, skipFilters;
		bool skipIfSubtitlesExists;
};

class GetFilesThread : public NapiThread
{
	Q_OBJECT
	signals:
		void fileNameChange(const QString & newfileName);
		void progressChange(int newValue);
		void downloadFinished(bool interrupt);

	public:
		void run();

		QStringList queue, gotList;
		int napiSuccess, napiFail;
};

class frmScan: public QDialog
{
Q_OBJECT

	public:
		frmScan(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmScan(){};

	private:
		void closeEvent(QCloseEvent *event);

		Ui::frmScan ui;
		ScanFilesThread scanThread;
		GetFilesThread getThread;

	private slots:
		bool pbCancelClicked();
		void selectDirectory();
		void leDirectoryTextChanged();
		void pbScanClicked();
		void addFile(const QString & fileName);
		void scanFinished();
		void enableControlWidgets(bool enable);
		void enableFilesWidgets(bool enable);
		void pbAddAllClicked();
		void pbAddClicked();
		void pbRemoveClicked();
		void pbRemoveAllClicked();
		void checkPbGetEnabled();
		void pbGetClicked();
		void fileNameChange(const QString & fileName);
		void downloadFinished(bool interrupt);
};

#endif

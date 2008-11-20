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
#include <QtGui>

#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiprojektengine.h"
#include "qnapiopendialog.h"

#include "frmsummary.h"


class ScanFilesThread : public QNapiThread
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


class GetFilesThread : public QNapiThread
{
	Q_OBJECT
	signals:
		void fileNameChange(const QString & newfileName);
		void progressChange(int newValue);
		void criticalError(const QString & message);

	private slots:
		void setCriticalMessage(const QString & msg)
		{
			criticalMessage = msg;
		}

	public:
		GetFilesThread()
		{
			connect(this, SIGNAL(criticalError(const QString &)),
					this, SLOT(setCriticalMessage(const QString &)));
		}
		void run();

		QStringList queue, gotList, failedList;
		int napiSuccess, napiFail;
		QString criticalMessage;
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
		bool closeRequested;

        QIcon iconFilm;

	private slots:
		bool pbCancelClicked();
		void selectDirectory();
		void leDirectoryTextChanged();
		void pbScanClicked();
		void addFile(const QString & fileName);
		void scanFinished();
		void enableControlWidgets(bool enable);
		void enableFilesWidgets(bool enable);
		void lwMoviesClicked(QListWidgetItem * item);
		void tbSelectAllClicked();
		void tbUnselectAllClicked();
		void tbInvertSelectionClicked();
		void checkPbGetEnabled();
		void pbGetClicked();
		void fileNameChange(const QString & fileName);
		void downloadFinished();
};

#endif

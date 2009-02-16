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

#ifndef __FRMPROGRESS__H__
#define __FRMPROGRESS__H__

#include "ui_frmprogress.h"
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QMutex>

#include "frmsummary.h"

#include "qnapi.h"
#include "qnapithread.h"
#include "qnapiconfig.h"
#include "qnapiopendialog.h"
#include "frmlistsubtitles.h"

class GetThread : public QNapiThread
{
	Q_OBJECT

	signals:
		void fileNameChange(const QString & newfileName);
		void actionChange(const QString & newAction);
		void progressChange(int current, int all, float stageProgress);
		void criticalError(const QString & message);
		void selectSubtitles(QString fileName, QNapiSubtitleInfoList subtitles);

	private slots:
		void setCriticalMessage(const QString & msg)
		{
			criticalMessage = msg;
		}
		void subtitlesSelected(int idx);

	public:
		GetThread()
		{
			connect(this, SIGNAL(criticalError(const QString &)),
					this, SLOT(setCriticalMessage(const QString &)));
		}
		void setEngines(QStringList enginesList) {engines = enginesList;}
		void setLanguage(QString language){ lang = language; }
		void run();

		QStringList queue, gotList, failedList, engines;
		QString lang;
		int napiSuccess, napiFail;
		QString criticalMessage;
		QMutex waitForDlg;
		int selIdx;
};

class frmProgress: public QWidget
{
	Q_OBJECT

	public:
		frmProgress(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmProgress(){};
		void setEngines(QStringList enginesList)
		{
			getThread.setEngines(enginesList);
		}

	signals:
		void subtitlesSelected(int idx);

	public slots:

		void receiveRequest(const QString & request);
		void enqueueFile(const QString &file);
		void enqueueFiles(const QStringList &fileList);
		bool download();
		void updateProgress(int current, int all, float stageProgress);
		void selectSubtitles(QString fileName, QNapiSubtitleInfoList subtitles);
		void downloadFinished();

		void setBatchMode(bool value) { batchMode = value; }
		void setBatchLanguage(QString lang) { getThread.setLanguage(lang); }
		bool isBatchMode() { return batchMode; }

	private:
		void closeEvent(QCloseEvent *event);
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);

		Ui::frmProgress ui;

		GetThread getThread;

		bool batchMode, showSummary, closeRequested;
		QMutex mutex;
};

#endif

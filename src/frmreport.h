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

#ifndef __FRMREPORT__H__
#define __FRMREPORT__H__

#include "ui_frmreport.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopWidget>

#include "napi.h"
#include "napithread.h"
#include "qnapiconfig.h"

class ReportThread : public NapiThread
{
Q_OBJECT
	public:
		void run();
		void setReportParams(const QString & movie_file, const QString & lang,
								const QString & comment_txt)
		{
			movie = movie_file;
			language = lang;
			comment = comment_txt;
		}

		napiReportResult taskResult;

	signals:
		void reportFinished();
		void serverMessage(QString msg);
		void invalidUserPass();

	private:
		QString movie, language, comment;
};

class frmReport: public QDialog
{
Q_OBJECT
	public:
		frmReport(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmReport() {};

	private:
		void closeEvent(QCloseEvent *event);

		Ui::frmReport ui;

		ReportThread reportThread;

	private slots:
		void selectMovie();
		void checkReportEnable();
		void cbProblemChanged();
		void pbReportClicked();
		void reportFinished(bool interrupt = false);
		void serverMessage(QString msg);
		void invalidUserPass();
};

#endif

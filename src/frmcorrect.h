/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef __FRMCORRECT__H__
#define __FRMCORRECT__H__

#include "ui_frmcorrect.h"
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

class PostThread : public NapiThread
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

		napiUploadResult taskResult;

	signals:
		void postFinished();
		void invalidUserPass();

	private:
		QString movie, subtitles, comment, language;
};

class frmCorrect: public QDialog
{
Q_OBJECT
	public:
		frmCorrect(QWidget *parent = 0, Qt::WFlags f = 0);
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

/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

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

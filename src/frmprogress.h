/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __FRMPROGRESS__H__
#define __FRMPROGRESS__H__

#include "ui_frmprogress.h"
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMenu>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QUrl>

#include "frmabout.h"
#include "frmoptions.h"
#include "frmupload.h"
#include "frmcorrect.h"
#include "frmreport.h"

#include "napi.h"
#include "napithread.h"
#include "qnapiconfig.h"

class GetThread : public NapiThread
{
	Q_OBJECT

	signals:
		void windowTitleChange(const QString & newTitle);
		void fileNameChange(const QString & newfileName);
		void actionChange(const QString & newAction);
		void progressChange(int newValue);

	public:
		void run();
		void setDebugMode(bool mode) { debugMode = mode; }

		QStringList queue;
		int napiSuccess, napiFail;

	private:
		bool debugMode;
};

class frmProgress: public QWidget
{
	Q_OBJECT

	public:
		frmProgress(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmProgress(){};

		void createTrayIcon();

	public slots:

		void showUploadDialog();
		void showCorrectDialog();
		void showReportDialog();
		void showOptions();
		void showAbout();
		void downloadFinished();
		void showOpenDialog();
		void enqueueFiles(const QStringList &fileList);
		void setBatchMode(bool value) { batchMode = value; }
		bool isBatchMode() { return batchMode; }
		void setQuietMode(bool value) { quietMode = value; }
		bool isQuietMode() { return quietMode; }
		void setConsoleMode(bool value) { consoleMode = value; setQuietMode(value); getThread.setDebugMode(value); }
		bool download();

	private:
		void closeEvent(QCloseEvent *event);

		Ui::frmProgress ui;

		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu, *napiSubMenu;
		QAction *getAction, *addNewAction, *addCorrectedAction, *reportBadAction,
				*optionsAction, *aboutAction, *quitAction;

		GetThread getThread;

		bool batchMode;
		bool quietMode;
		bool consoleMode;
		
	private slots:
		void updatePreviousPath(const QString & path);
};

#endif
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
#include "frmscan.h"

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
		void setVerboseMode(bool mode) { verboseMode = mode; }

		QStringList queue;
		int napiSuccess, napiFail;

	private:
		bool verboseMode;
};

class frmProgress: public QWidget
{
	Q_OBJECT

	public:
		frmProgress(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmProgress(){};

		void createTrayIcon();

	public slots:

		void showScanDialog();
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
		void setConsoleMode(bool value) {
			consoleMode = value; setQuietMode(value); getThread.setVerboseMode(value);
		}
		bool download();
		void downloadFile(QString fileName);

	private:
		void closeEvent(QCloseEvent *event);

		Ui::frmProgress ui;

		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu, *napiSubMenu;
		QAction *getAction, *scanAction, *addNewAction, *addCorrectedAction,
				*reportBadAction, *optionsAction, *aboutAction, *quitAction;

		GetThread getThread;

		bool batchMode;
		bool quietMode;
		bool consoleMode;
		
	private slots:
		void updatePreviousPath(const QString & path);
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif

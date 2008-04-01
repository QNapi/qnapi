/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __FRMSCAN__H__
#define __FRMSCAN__H__

#include "ui_frmscan.h"
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
#include <QVariant>

#include "napithread.h"
#include "napi.h"
#include "qnapiconfig.h"

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

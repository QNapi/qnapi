/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmscan.h"

frmScan::frmScan(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, true);
#endif
	setAttribute(Qt::WA_QuitOnClose, false);

	connect(ui.pbDirectorySelect, SIGNAL(clicked()), this, SLOT(selectDirectory()));
	connect(ui.leDirectory, SIGNAL(textChanged(QString)), this, SLOT(leDirectoryTextChanged()));
	connect(ui.pbScan, SIGNAL(clicked()), this, SLOT(pbScanClicked()));
	connect(&scanThread, SIGNAL(addFile(QString)), this, SLOT(addFile(QString)));
	connect(&scanThread, SIGNAL(scanFinished(bool)), this, SLOT(scanFinished()));
	connect(ui.pbAddAll, SIGNAL(clicked()), this, SLOT(pbAddAllClicked()));
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(pbAddClicked()));
	connect(ui.pbRemove, SIGNAL(clicked()), this, SLOT(pbRemoveClicked()));
	connect(ui.pbRemoveAll, SIGNAL(clicked()), this, SLOT(pbRemoveAllClicked()));
	connect(ui.lwFound, SIGNAL(dragFinished()), this, SLOT(checkPbGetEnabled()));
	connect(ui.lwSelected, SIGNAL(dragFinished()), this, SLOT(checkPbGetEnabled()));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmScan::resizeEvent(QResizeEvent *resize)
{
	int b = 10;
	ui.gridLayout->setGeometry(QRect(b, b, resize->size().width() - b*2,
								resize->size().height() - b*2));
}

void frmScan::selectDirectory()
{
	QString propDir = QFileInfo(ui.leDirectory->text()).path();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Wskaż katalog do przeskanowania"),
					QDir().exists(propDir) ? propDir  : QDir::home().path(),
					QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(!dir.isEmpty() && QDir().exists(dir))
		ui.leDirectory->setText(dir);
}

void frmScan::leDirectoryTextChanged()
{
		ui.pbScan->setEnabled(!ui.leDirectory->text().isEmpty()
							? QDir().exists(ui.leDirectory->text())
							: false);
}

void frmScan::pbScanClicked()
{
	if(!scanThread.isRunning())
	{
		ui.leDirectory->setEnabled(false);
		ui.pbDirectorySelect->setEnabled(false);
		ui.pbScan->setText(tr("Przerwij"));
		ui.lbAction->setText(tr("Skanowanie folderów..."));
		ui.pbProgress->setEnabled(false);
		ui.pbGet->setEnabled(false);
		ui.lwFound->clear();
		ui.lwSelected->clear();
		enableControlWidgets(false);
		enableFilesWidgets(false);

		scanThread.setSearchPath(ui.leDirectory->text());
		scanThread.setFilters(ui.cbFilters->currentText());
		scanThread.setSkipFilters(ui.leSkipFilters->text());
		scanThread.setSkipIfSubtitlesExists(ui.cbSkipIfSubtitlesExists->isChecked());

		scanThread.start();
	}
	else
	{
		scanThread.requestAbort();
		ui.lbAction->setText(tr("Przerywanie skanowania..."));
		ui.pbScan->setEnabled(false);
		qApp->processEvents();

		scanThread.wait();
		ui.pbScan->setEnabled(true);
		scanFinished();
	}
}

void frmScan::addFile(const QString & fileName)
{
	ui.lwFound->addItem(fileName);
}

void frmScan::scanFinished()
{
	enableControlWidgets(true);
	enableFilesWidgets(ui.lwFound->count() > 0);
	ui.pbScan->setText("Skanuj");
	ui.lbAction->setText(tr((ui.lwFound->count() > 0)
							? "Przenieś pliki z listy znalezionych do listy wybranych."
							: "Nie znaleziono plików z filmami."));
}

void frmScan::enableControlWidgets(bool enable)
{
	ui.leDirectory->setEnabled(enable);
	ui.pbDirectorySelect->setEnabled(enable);
	ui.lbFilter->setEnabled(enable);
	ui.cbFilters->setEnabled(enable);
	ui.lbSkip->setEnabled(enable);
	ui.leSkipFilters->setEnabled(enable);
	ui.cbSkipIfSubtitlesExists->setEnabled(enable);
}

void frmScan::enableFilesWidgets(bool enable)
{
	ui.lbFound->setEnabled(enable);
	ui.lwFound->setEnabled(enable);
	ui.pbAddAll->setEnabled(enable);
	ui.pbAdd->setEnabled(enable);
	ui.pbRemove->setEnabled(enable);
	ui.pbRemoveAll->setEnabled(enable);
	ui.lbSelected->setEnabled(enable);
	ui.lwSelected->setEnabled(enable);
}

void frmScan::pbAddAllClicked()
{
	ui.lwFound->MoveAll(ui.lwSelected);
	checkPbGetEnabled();
}

void frmScan::pbAddClicked()
{
	ui.lwFound->MoveSelected(ui.lwSelected);
	checkPbGetEnabled();
}

void frmScan::pbRemoveClicked()
{
	ui.lwSelected->MoveSelected(ui.lwFound);
	checkPbGetEnabled();
}

void frmScan::pbRemoveAllClicked()
{
	ui.lwSelected->MoveAll(ui.lwFound);
	checkPbGetEnabled();
}

void frmScan::checkPbGetEnabled()
{
	ui.pbGet->setEnabled(ui.lwSelected->count() > 0);
}

void ScanFilesThread::run()
{
	abort = false;
	fileList.clear();

	emit scanFinished(doScan(searchPath));
}

bool ScanFilesThread::doScan(const QString & path)
{
	QString myPath = QFileInfo(path).absoluteFilePath();
	if(!QDir().exists(myPath))
		return false;

	emit folderChange(myPath);

	QFileInfoList list = QDir(myPath).entryInfoList(scanFilters, QDir::AllDirs | QDir::Files
							| QDir::NoDotAndDotDot | QDir::Readable | QDir::Hidden);

	for(QFileInfoList::iterator p=list.begin(); p != list.end(); p++)
	{
		if(abort) return false;

		if((*p).isDir() && ((*p).absoluteFilePath() != myPath))
		{
			if(!doScan((*p).absoluteFilePath()))
				return false;
		}
		else
		{
			if(!QFile::exists((*p).absoluteFilePath())) continue;

			if(skipIfSubtitlesExists &&
				QFile::exists((*p).absolutePath() + "/" + (*p).completeBaseName() + ".txt"))
			{
				continue;
			}

			bool skip = false;
			for(QStringList::iterator s = skipFilters.begin(); s != skipFilters.end(); s++)
			{
				if((*s).isEmpty()) continue;
				if((*p).fileName().indexOf(*s) >= 0)
				{
					skip = true;
					break;
				}
			}

			if(skip) continue;

			fileList << (*p).absoluteFilePath();
			emit addFile((*p).absoluteFilePath());
		}
	}

	return true;
}

void GetFilesThread::run()
{
	int size = queue.size();

	if(size <= 0) return;

	float step = 100.0f / size;
	QString windowTitle, md5;
	napiSuccess = napiFail = 0;

	QString tmpZip =  GlobalConfig().tmpPath() + "/QNapi.napisy.7z";

	emit progressChange(0);

	for(int i = 0; i < size; i++)
	{
		QFileInfo fi(queue[i]);
		emit fileNameChange(fi.fileName());

		emit actionChange(tr("Obliczanie sumy kontrolnej pliku..."));

		md5 = napiFileMd5Sum(queue[i], NAPI_10MB);
		if(abort) return;

		emit progressChange((int)ceil(step * i + step / 3));
		emit actionChange(tr("Pobieranie napisów dla pliku..."));

		// pobieranie
		if(!napiDownload(md5, tmpZip, GlobalConfig().language(),
							GlobalConfig().nick(), GlobalConfig().pass()))
		{
			if(abort) return;
			++napiFail;
			continue;
		}

		if(abort) return;

		emit progressChange((int)ceil(step * i + 2 * step / 3));
		emit actionChange(tr("Dopasowywanie napisów..."));

		// dopasowywanie
		if(!napiMatchSubtitles(md5, tmpZip, queue[i], GlobalConfig().noBackup(),
								GlobalConfig().tmpPath(), GlobalConfig().p7zipPath()))
		{
			if(abort) return;
			++napiFail;
			continue;
		}

		if(abort) return;

		++napiSuccess;

		if(GlobalConfig().changeEncoding())
		{
			emit progressChange((int)ceil(step * i + 5 * step / 6));
			emit actionChange(tr("Zmiana kodowania napisów..."));

			// Jesli automatycznie nie uda mu sie wykryc kodowania, to jako kodowania
			// zrodlowego uzywa kodowania wybranego przez uzytkownika
			if (!GlobalConfig().autoDetectEncoding()
				|| !napiConvertFile(QFileInfo(queue[i]).path() + "/" +
								QFileInfo(queue[i]).completeBaseName() + ".txt",
								GlobalConfig().encodingTo())
				)
			{
				napiConvertFile(QFileInfo(queue[i]).path() + "/" +
								QFileInfo(queue[i]).completeBaseName() + ".txt",
								GlobalConfig().encodingFrom(), GlobalConfig().encodingTo());
			}

			if(abort) return;
		}

		emit progressChange((int)ceil(step * (i + 1)));
	}

	emit progressChange(100);
}


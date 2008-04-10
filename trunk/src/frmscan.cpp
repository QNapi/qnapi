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

#include "frmscan.h"

frmScan::frmScan(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_QuitOnClose, false);

	connect(ui.pbCancel, SIGNAL(clicked()), this, SLOT(pbCancelClicked()));
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
	connect(ui.pbGet, SIGNAL(clicked()), this, SLOT(pbGetClicked()));
	connect(&getThread, SIGNAL(fileNameChange(QString)), this, SLOT(fileNameChange(QString)));
	connect(&getThread, SIGNAL(progressChange(int)), ui.pbProgress, SLOT(setValue(int)));
	connect(&getThread, SIGNAL(downloadFinished(bool)), this, SLOT(downloadFinished(bool)));

	if(QFileInfo(GlobalConfig().lastScanDir()).isDir())
		ui.leDirectory->setText(GlobalConfig().lastScanDir());

	QList<QVariant> scanFilters = GlobalConfig().scanFilters();
	ui.cbFilters->clear();

	for(int i = 0; i < scanFilters.size(); i++)
	{
		ui.cbFilters->addItem(scanFilters[i].toString());
	}

	ui.leSkipFilters->setText(GlobalConfig().scanSkipFilters());
	ui.cbSkipIfSubtitlesExists->setChecked(GlobalConfig().scanSkipIfSubtitlesExists());

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmScan::closeEvent(QCloseEvent *event)
{
	QList<QVariant> scanFilters;
	for(int i = 0; i < ui.cbFilters->count(); i++)
	{
		scanFilters << ui.cbFilters->itemText(i);
	}

	GlobalConfig().setLastScanDir(ui.leDirectory->text());
	GlobalConfig().setScanFilters(scanFilters);
	GlobalConfig().setScanSkipFilters(ui.leSkipFilters->text());
	GlobalConfig().setScanSkipIfSubtitlesExists(ui.cbSkipIfSubtitlesExists->isChecked());
	GlobalConfig().save();

	if(pbCancelClicked())
		event->accept();
	else
		event->ignore();
}

bool frmScan::pbCancelClicked()
{
	if(scanThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać skanowanie katalogów?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			scanThread.requestAbort();
			ui.lbAction->setText(tr("Kończenie zadań..."));
			qApp->processEvents();
			scanThread.terminate();
			scanThread.wait();
			return true;
		}
		return false;
	}

	if(getThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać pobieranie napisów?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			getThread.requestAbort();
			ui.lbAction->setText(tr("Kończenie zadań..."));
			qApp->processEvents();
			getThread.terminate();
			getThread.wait();
			downloadFinished(true);
			return true;
		}
		return false;
	}
	
	close();
	return true;
}

void frmScan::selectDirectory()
{
	QString propDir = QFileInfo(ui.leDirectory->text()).path();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Wskaż katalog do przeskanowania"),
					QDir().exists(propDir) ? propDir  : GlobalConfig().previousDialogPath(),
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
	ui.lbDirectory->setEnabled(enable);
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
	ui.pbProgress->setEnabled(ui.lwSelected->count() > 0);
}

void frmScan::pbGetClicked()
{
	if(!getThread.isRunning())
	{
		enableControlWidgets(false);
		enableFilesWidgets(false);
		ui.pbScan->setEnabled(false);
		ui.pbGet->setText(tr("Przerwij"));
		ui.pbProgress->setValue(0);

		getThread.queue.clear();
		for(int i = 0; i < ui.lwSelected->count(); i++)
		{
			getThread.queue << ui.lwSelected->item(i)->text();
		}
		getThread.start();
	}
	else
	{
		getThread.requestAbort();
		ui.lbAction->setText(tr("Przerywanie pobierania..."));
		ui.pbGet->setEnabled(false);
		qApp->processEvents();

		getThread.terminate();
		getThread.wait();

		downloadFinished(true);
	}
}

void frmScan::fileNameChange(const QString & fileName)
{
	ui.lbAction->setText(tr("Pobieranie napisów dla <b>%1</b>...").arg(fileName));
}

void frmScan::downloadFinished(bool interrupt)
{
	enableControlWidgets(true);
	enableFilesWidgets(true);
	ui.pbGet->setEnabled(true);
	ui.pbScan->setEnabled(true);
	ui.pbGet->setText(tr("Pobierz napisy"));
	ui.pbProgress->setValue(0);
	

	if(getThread.napiSuccess + getThread.napiFail > 0)
	{
		ui.lbAction->setText(tr("Napisy pobrano."));

		if(getThread.napiSuccess > 0)
		{
			frmSummary summary;
			summary.setFileList(getThread.gotList);
			summary.setFailedCount(getThread.napiFail);
			summary.exec();
		}
		else
		{
			QString msg = tr("Nie udało się dopasować napisów dla %1 %2!").arg(getThread.napiFail)
							.arg(tr((getThread.napiFail == 1) ? "pliku" : "plików"));
			QMessageBox::information(0, tr("Zakończono pobieranie napisów"), msg);
		}
	}

	if(interrupt)
		ui.lbAction->setText(tr("Przerwano."));
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
	abort = false;
	gotList.clear();
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

		md5 = napiFileMd5Sum(queue[i], NAPI_10MB);
		if(abort) return;

		emit progressChange((int)ceil(step * i + step / 3));

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
		gotList << queue[i];

		if(GlobalConfig().changeEncoding())
		{
			emit progressChange((int)ceil(step * i + 5 * step / 6));

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
	emit downloadFinished(false);
}

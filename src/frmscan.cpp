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
	connect(&getThread, SIGNAL(finished()), this, SLOT(downloadFinished()));

	if(QFileInfo(GlobalConfig().lastScanDir()).isDir())
		ui.leDirectory->setText(GlobalConfig().lastScanDir());

	QList<QString> scanFilters = GlobalConfig().scanFilters();
	ui.cbFilters->clear();

	for(int i = 0; i < scanFilters.size(); i++)
	{
		ui.cbFilters->addItem(scanFilters[i]);
	}

	ui.leSkipFilters->setText(GlobalConfig().scanSkipFilters());
	ui.cbSkipIfSubtitlesExists->setChecked(GlobalConfig().scanSkipIfSubtitlesExists());

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmScan::closeEvent(QCloseEvent *event)
{
	QList<QString> scanFilters;
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
	closeRequested = true;
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
			pbGetClicked();
		}
		return false;
	}

	close();
	return true;
}

void frmScan::selectDirectory()
{
	QString dir = QFileInfo(ui.leDirectory->text()).path();
	dir = QDir().exists(dir) ? dir : GlobalConfig().lastScanDir();

	QNapiOpenDialog openDialog(this, tr("Wskaż katalog do skanowania"),
								dir, QNapiOpenDialog::None);

	if(openDialog.selectDirectory())
		dir = openDialog.selectedFiles().first();

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
		closeRequested = false;
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
	}
}

void frmScan::fileNameChange(const QString & fileName)
{
	ui.lbAction->setText(tr("Pobieranie napisów dla <b>%1</b>...").arg(fileName));
}

void frmScan::downloadFinished()
{
	enableControlWidgets(true);
	enableFilesWidgets(true);
	ui.pbGet->setEnabled(true);
	ui.pbScan->setEnabled(true);
	ui.pbGet->setText(tr("Pobierz napisy"));
	ui.pbProgress->setValue(0);
	
	if(!getThread.criticalMessage.isEmpty())
	{
		ui.lbAction->setText(getThread.criticalMessage);
		QMessageBox::critical(0, tr("Błąd krytyczny!"), getThread.criticalMessage);
	}
	else if(getThread.napiSuccess + getThread.napiFail > 0)
	{
		ui.lbAction->setText(tr("Napisy pobrano."));

		if(getThread.napiSuccess > 0)
		{
			frmSummary summary;
			summary.setSuccessList(getThread.gotList);
			summary.setFailedList(getThread.failedList);
			summary.exec();
		}
		else
		{
			QString msg = tr("Nie udało się dopasować napisów dla %1 %2!").arg(getThread.napiFail)
							.arg(tr((getThread.napiFail == 1) ? "pliku" : "plików"));
			QMessageBox::information(0, tr("Zakończono pobieranie napisów"), msg);
		}
	}
	else
		ui.lbAction->setText(tr("Zakończono."));

	if(closeRequested)
		close();
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
	criticalMessage.clear();
	gotList.clear();
	failedList.clear();
	int size = queue.size();

	if(size <= 0) return;

	float step = 100.0f / size;
	QString windowTitle, md5;
	napiSuccess = napiFail = 0;

	QNapiProjektEngine *napi;

	emit progressChange(0);

	for(int i = 0; i < size; i++)
	{
		napi = new QNapiProjektEngine(queue[i]);
		if(!napi) continue;

		QFileInfo fi(queue[i]);
		emit fileNameChange(fi.fileName());

		if(!napi->checkWritePermissions())
		{
			emit criticalError(tr("Brak uprawnień zapisu do katalogu '%1'!").arg(QFileInfo(queue[i]).path()));
			delete napi;
			break;
		}

		napi->checksum();
		if(abort)
		{
			delete napi;
			return;
		}

		emit progressChange((int)ceil(step * i + step / 3));

		// pobieranie
		if(!napi->tryDownload())
		{
			if(abort)
			{
				delete napi;
				return;
			}
			++napiFail;
			failedList << queue[i];
			delete napi;
			continue;
		}

		if(abort)
		{
			delete napi;
			return;
		}

		emit progressChange((int)ceil(step * i + 2 * step / 3));

		// dopasowywanie
		if(!napi->tryMatch())
		{
			if(abort)
			{
				delete napi;
				return;
			}

			++napiFail;
			failedList << queue[i];
			delete napi;
			continue;
		}

		if(abort)
		{
			delete napi;
			return;
		}

		++napiSuccess;
		gotList << queue[i];

		if(GlobalConfig().ppEnabled())
		{
			emit progressChange((int)ceil(step * i + 5 * step / 6));
			napi->doPostProcessing();
		}

		emit progressChange((int)ceil(step * (i + 1)));
		delete napi;
	}

	emit progressChange(100);
}

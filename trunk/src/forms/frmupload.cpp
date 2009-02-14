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

#include "frmupload.h"

frmUpload::frmUpload(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_QuitOnClose, false);

	connect(ui.pbSelectDirectory, SIGNAL(clicked()), this, SLOT(selectDirectory()));
	connect(ui.leSelectDirectory, SIGNAL(textChanged(QString)), this, SLOT(leSelectDirectoryChanged()));
	connect(ui.pbScan, SIGNAL(clicked()), this, SLOT(pbScanClicked()));
	connect(&scanThread, SIGNAL(scanFinished(bool)), this, SLOT(scanFinished(bool)));
	connect(&scanThread, SIGNAL(folderChange(QString)), this, SLOT(folderChange(QString)));
	connect(ui.pbUpload, SIGNAL(clicked()), this, SLOT(pbUploadClicked()));
	connect(&uploadThread, SIGNAL(finished()), this, SLOT(uploadFinished()));
	connect(&uploadThread, SIGNAL(progressChange(int)), ui.pbProgress, SLOT(setValue(int)));
	connect(&uploadThread, SIGNAL(fileNameChange(QString)), this, SLOT(fileNameChange(QString)));
	connect(&uploadThread, SIGNAL(checkingUserPass()), this, SLOT(checkingUserPass()));
	connect(&uploadThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));
	

	if(QFileInfo(GlobalConfig().previousDialogPath()).isDir())
		ui.leSelectDirectory->setText(GlobalConfig().previousDialogPath());

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmUpload::closeEvent(QCloseEvent *event)
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
			event->accept();
		}
		else
		{
			event->ignore();
		}
		return;
	}
	else if(uploadThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać wysyłanie napisów?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			pbUploadClicked();
		}
		event->ignore();
	}
	else
		event->accept();
}

void frmUpload::selectDirectory()
{
	QString dir = QFileInfo(ui.leSelectDirectory->text()).path();
	dir = QDir().exists(dir) ? dir : GlobalConfig().previousDialogPath();

	QNapiOpenDialog openDialog(this, tr("Wskaż katalog do skanowania"),
								dir, QNapiOpenDialog::None);

	if(openDialog.selectDirectory())
		dir = openDialog.selectedFiles().first();

	if(!dir.isEmpty() && QDir().exists(dir))
		ui.leSelectDirectory->setText(dir);
}

void frmUpload::leSelectDirectoryChanged()
{
	ui.pbScan->setEnabled(!ui.leSelectDirectory->text().isEmpty()
							? QDir().exists(ui.leSelectDirectory->text())
							: false);
}

void frmUpload::pbScanClicked()
{
	if(!scanThread.isRunning())
	{
		ui.leSelectDirectory->setEnabled(false);
		ui.pbSelectDirectory->setEnabled(false);
		ui.pbScan->setText(tr("Przerwij"));
		ui.lbAction->setText(tr("Skanowanie katalogów..."));
		ui.lbFoldersCount->setEnabled(false);
		ui.lbFoldersCount->setText(tr("Katalogów: <b>0</b>"));
		ui.lbMoviesCount->setEnabled(false);
		ui.lbMoviesCount->setText(tr("Filmów: <b>0</b>"));
		ui.lbSubtitlesCount->setEnabled(false);
		ui.lbSubtitlesCount->setText(tr("Napisów: <b>0</b>"));
		ui.pbProgress->setEnabled(false);
		ui.pbUpload->setEnabled(false);
		ui.cbFollowSymLinks->setEnabled(false);

		scanThread.setSearchPath(ui.leSelectDirectory->text());
		scanThread.setFollowSymLinks(ui.cbFollowSymLinks->isChecked());
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
		scanFinished(false);
	}
}

void frmUpload::scanFinished(bool result)
{
	ui.leSelectDirectory->setEnabled(true);
	ui.pbSelectDirectory->setEnabled(true);
	ui.cbFollowSymLinks->setEnabled(true);
	ui.pbScan->setText(tr("Skanuj"));
	ui.lbAction->setText("");
	
	if(result)
	{
		if(scanThread.fileList.size() == 0)
		{
			QMessageBox::warning(this, tr("Nie znaleziono napisów"),
								tr("W wybranym katalogu nie znaleziono żadnych napisów!"));
		}
		else
		{
			ui.lbAction->setText(tr("Teraz możesz wysłać napisy na serwer."));
			ui.lbFoldersCount->setEnabled(true);
			ui.lbFoldersCount->setText(tr("Katalogów: ") + "<b>"
								+ QString::number(scanThread.folders) + "</b>");
			ui.lbMoviesCount->setEnabled(true);
			ui.lbMoviesCount->setText(tr("Filmów: ") + "<b>"
								+ QString::number(scanThread.movies) + "</b>");
			ui.lbSubtitlesCount->setEnabled(true);
			ui.lbSubtitlesCount->setText(tr("Napisów: ") + "<b>"
								+ QString::number(scanThread.subtitles) + "</b>");
			ui.pbProgress->setEnabled(true);
			ui.pbUpload->setEnabled(true);
		}
	}
}

void frmUpload::pbUploadClicked()
{
	if(!uploadThread.isRunning())
	{
		ui.lbSelectDirectory->setEnabled(false);
		ui.leSelectDirectory->setEnabled(false);
		ui.pbSelectDirectory->setEnabled(false);
		ui.pbScan->setEnabled(false);
		ui.pbUpload->setText(tr("Przerwij wysyłanie"));
		ui.pbProgress->setValue(0);

		uploadThread.setMovieList(scanThread.fileList);
		uploadThread.start();
	}
	else
	{
		uploadThread.requestAbort();
		ui.lbAction->setText(tr("Przerywanie wysyłania..."));
		ui.pbUpload->setEnabled(false);
		qApp->processEvents();
	}
}

void frmUpload::folderChange(const QString & folder)
{
	ui.lbAction->setText(tr("Skanowanie katalogu <b>") + folder + "</b>");
}

void frmUpload::fileNameChange(const QString & filename)
{
	ui.lbAction->setText(tr("Wysyłanie napisów dla <b>") + filename + "</b>");
}

void frmUpload::checkingUserPass()
{
	ui.lbAction->setText(tr("Sprawdzanie nazwy użytkownika i hasła..."));
}

void frmUpload::uploadFinished()
{
	ui.lbSelectDirectory->setEnabled(true);
	ui.leSelectDirectory->setEnabled(true);
	ui.pbSelectDirectory->setEnabled(true);
	ui.pbScan->setEnabled(true);
	ui.pbUpload->setEnabled(true);
	ui.pbUpload->setText(tr("Wyślij napisy do serwera"));
	ui.pbProgress->setValue(0);

	if(!uploadThread.invalidUP)
	{
		ui.lbAction->setText(tr("Zakończono wysyłanie napisów."));

		QString msg = tr("Wysłano napisów: %1\n").arg(uploadThread.added_new+uploadThread.added_ok)
				+ tr("W tym zupełnie nowych: %1\n").arg(uploadThread.added_new)
				+ tr("Nie udało się wysłać: %1\n").arg(uploadThread.failed);

		QMessageBox::information(this, tr("Rezultat wysyłania"), msg);
	}
}

void frmUpload::invalidUserPass()
{
	ui.lbAction->setText(tr("Nazwa użytkownika lub hasło jest niepoprawne."));
	QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void ScanThread::run()
{
	abort = false;
	folders = movies = subtitles = 0;
	fileList.clear();
	visited.clear();
	searchFilters.clear();
	searchFilters << "*.avi" << "*.asf" << "*.divx" << "*.dat" << "*.mkv" << "*.mov" << "*.mp4"
					<< "*.mpeg" << "*.mpg" << "*.ogm" << "*.rm" << "*.rmvb" << "*.wmv";

	QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot
							| QDir::Readable | QDir::Hidden;

	if(!followSymLinks)
		filters |= QDir::NoSymLinks;

	emit scanFinished(doScan(searchPath, filters));
}

bool ScanThread::doScan(const QString & path, QDir::Filters filters)
{
	QString myPath = QFileInfo(path).absoluteFilePath();

	if(!QDir().exists(myPath))
		return false;

	QString myCPath = QDir(path).canonicalPath();

	if(visited.contains(myCPath))
		return true;

	visited << myCPath;

	emit folderChange(myPath);
	++folders;

	QFileInfoList list = QDir(myPath).entryInfoList(searchFilters, filters);

	for(QFileInfoList::iterator p=list.begin(); p != list.end(); p++)
	{
		if(abort) return false;

		if((*p).isDir() && ((*p).absoluteFilePath() != myPath))
		{
			if(!doScan((*p).absoluteFilePath(), filters))
				return false;
		}
		else
		{
			if(!QFile::exists((*p).absoluteFilePath())) continue;
			++movies;

			if(QFile::exists((*p).absolutePath() + "/" + (*p).completeBaseName() + ".txt"))
			{
				++subtitles;
				fileList << (*p).absoluteFilePath();
			}
		}
	}

	return true;
}

void UploadThread::run()
{
	added_new = added_ok = failed = 0;
	abort = false;
	invalidUP = false;

	emit checkingUserPass();

	if(!QNapiProjektEngine::checkUser(	GlobalConfig().nick("NapiProjekt"),
										GlobalConfig().pass("NapiProjekt")))
	{
		invalidUP = true;
		emit invalidUserPass();
		return;
	}

	if(abort) return;

	int size = movieList.size();
	QNapiProjektEngine *napi;

	for(int i = 0; i < size; i++)
	{
		QFileInfo fi(movieList[i]);
		napi = new QNapiProjektEngine(movieList[i], fi.path() + "/" + fi.completeBaseName() + ".txt");
		if(!napi) continue;

		emit fileNameChange(fi.fileName());
		emit progressChange(i * 100 / size);
		
		switch( napi->uploadSubtitles(	GlobalConfig().language(),
										GlobalConfig().nick("NapiProjekt"),
										GlobalConfig().pass("NapiProjekt"),
										false ) )
		{
			case QNapiProjektEngine::NAPI_ADDED_NEW: ++added_new; break;
			case QNapiProjektEngine::NAPI_FAIL: ++failed; break;
			default: ++added_ok; break;
		}

		delete napi;
		if(abort) break;
	}
	
	emit progressChange(100);
}

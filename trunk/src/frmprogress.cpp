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

#include "frmprogress.h"

frmProgress::frmProgress(QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	ui.setupUi(this);

	openDialog = 0;
	options = 0;
	createUser = 0;
	about = 0;
	scan = 0;
	upload = 0;
	correct = 0;
	report = 0;

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setBatchMode(false);
	setQuietMode(false);
	setConsoleMode(false);

	connect(&getThread, SIGNAL(fileNameChange(const QString &)),
			ui.lbFileName, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(actionChange(const QString &)),
			ui.lbAction, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(progressChange(int, int, float)),
			this, SLOT(updateProgress(int, int, float)));
	connect(&getThread, SIGNAL(finished()),
			this, SLOT(downloadFinished()));
}

void frmProgress::createTrayIcon()
{
	getAction = new QAction(tr("Pobierz"), this);
	connect(getAction, SIGNAL(triggered()), this, SLOT(showOpenDialog()));

	scanAction = new QAction(tr("Skanuj katalogi i dopasuj napisy"), this);
	connect(scanAction, SIGNAL(triggered()), this, SLOT(showScanDialog()));

	addNewAction = new QAction(tr("Dodaj nowe"), this);
	connect(addNewAction, SIGNAL(triggered()), this, SLOT(showUploadDialog()));

	addCorrectedAction = new QAction(tr("Dodaj poprawione"), this);
	connect(addCorrectedAction, SIGNAL(triggered()), this, SLOT(showCorrectDialog()));

	reportBadAction = new QAction(tr("Zgłoś niepasujące"), this);
	connect(reportBadAction, SIGNAL(triggered()), this, SLOT(showReportDialog()));

	optionsAction = new QAction(tr("Opcje"), this);
	connect(optionsAction, SIGNAL(triggered()), this, SLOT(showOptions()));

	createUserAction = new QAction(tr("Załóż konto"), this);
	connect(createUserAction, SIGNAL(triggered()), this, SLOT(showCreateUser()));

	aboutAction = new QAction(tr("O programie"), this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

	quitAction = new QAction(tr("Zakończ"), this);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

	napiSubMenu = new QMenu(this);
	napiSubMenu->setTitle(tr("Napisy"));
	napiSubMenu->addAction(getAction);
	napiSubMenu->addAction(scanAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(addNewAction);
	napiSubMenu->addAction(addCorrectedAction);
	napiSubMenu->addSeparator();
	napiSubMenu->addAction(reportBadAction);

	trayIconMenu = new QMenu(this);
	trayIconMenu->addMenu(napiSubMenu);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(optionsAction);
	trayIconMenu->addAction(createUserAction);
	trayIconMenu->addAction(aboutAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);

#ifdef Q_WS_MAC
	trayIcon->setIcon(QIcon(":/icon/qnapi-22-mono.png"));
#else
	trayIcon->setIcon(QIcon(":/icon/qnapi.png"));
#endif

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	trayIcon->show();
}

void frmProgress::receiveRequest(QString request)
{
	enqueueFile(request);
	if(!getThread.isRunning())
		download();
	raise();
	activateWindow();
}

void frmProgress::enqueueFile(const QString & file)
{
	static QMutex locker;
	locker.lock();
	if(QFile::exists(file))
	{
		getThread.queue << file;
		updateProgress(-1, getThread.queue.size(), -1);
	}
	locker.unlock();
}

void frmProgress::enqueueFiles(const QStringList & fileList)
{
	for(int i = 0; i < fileList.size(); i++)
	{
		enqueueFile(fileList.at(i));
	}
}

bool frmProgress::download()
{
	if(!QNapiAbstractEngine::checkP7ZipPath())
	{
		if(!quietMode)
			QMessageBox::warning(0, tr("Brak programu p7zip!"),
				tr("Ścieżka do programu p7zip jest nieprawidłowa!"));
		if(consoleMode)
			qDebug("%s", qPrintable(tr("Ścieżka do programu p7zip jest nieprawidłowa!")));
		return false;
	}

	if(!QNapiAbstractEngine::checkTmpPath())
	{
		if(!quietMode)
			QMessageBox::warning(0, tr("Nieprawidłowy katalog tymczasowy!"),
				tr("Nie można pisać do katalogu tymczasowego! Sprawdź swoje ustawienia."));
		if(consoleMode)
			qDebug("%s", qPrintable(tr("Nie można pisać do katalogu tymczasowego! Sprawdź swoje ustawienia.")));
		return false;
	}

	if(getThread.queue.isEmpty())
	{
		if(!quietMode)
			QMessageBox::warning(0, tr("Brak plików!"),
				tr("Nie wskazano filmów do pobrania napisów!"));
		if(consoleMode)
			qDebug("%s", qPrintable(tr("Nie wskazano filmów do pobrania napisów!")));
		return false;
	}

	if(!quietMode && !isVisible())
	{
		move((QApplication::desktop()->width() - width()) / 2, 
			(QApplication::desktop()->height() - height()) / 2);
		show();
	}

	showSummary = true;

	getThread.start();
	return true;
}

void frmProgress::updateProgress(int current, int all, float stageProgress)
{
	static int lastCurrent, lastAll;
	static float lastStageProgress;

	static QMutex m;
	m.lock();

	if(current >= 0) lastCurrent = current;
	if(all >= 0) lastAll = all;
	if(stageProgress >= 0) lastStageProgress = stageProgress;

	QString windowTitle = (lastAll > 1)
							? QString(tr("QNapi - pobieranie napisów (%1/%2)")).arg(lastCurrent + 1).arg(lastAll)
							: QString(tr("QNapi - pobieranie napisów..."));
	setWindowTitle(windowTitle);

	ui.pbProgress->setMaximum(lastAll * 100);
	ui.pbProgress->setValue(lastCurrent * 100 + (int)(lastStageProgress * 100));

	m.unlock();
}

void frmProgress::downloadFinished()
{
	hide();

	QStringList queue = getThread.queue;
	getThread.queue.clear();

	mutex.lock();
	if(showSummary)
	{
		if(queue.size() > 1)
		{
			if(!quietMode)
			{
				bool mac = 
#ifdef Q_WS_MAC
				true;
#else
				false;
#endif

				if(mac || batchMode)
				{
					if(getThread.napiSuccess > 0)
					{
						frmSummary summary;
						summary.setFileList(getThread.gotList);
						summary.setFailedCount(getThread.napiFail);
						summary.exec();
					}
					else
					{
						QString msg = tr("Nie udało się dopasować napisów dla %1 %2!")
										.arg(getThread.napiFail)
										.arg(tr((getThread.napiFail == 1) ? "pliku" : "plików"));
						QMessageBox::information(0, tr("Zakończono pobieranie napisów"), msg);
					}
				}
				else
				{
					QString msg;
					if(getThread.napiSuccess > 0)
						msg += tr("Dopasowano napisy dla %1 %2.%3").arg(getThread.napiSuccess)
								.arg(tr((getThread.napiSuccess > 1) ? "plików" : "pliku"))
								.arg((getThread.napiFail > 0) ? "\n" : "");
					if(getThread.napiFail > 0)
						msg += tr("Nie udało się dopasować napisów dla %1 %2!").arg(getThread.napiFail)
								.arg(tr((getThread.napiFail > 1) ? "plików" : "pliku"));
#ifndef Q_WS_MAC
					if(QSystemTrayIcon::supportsMessages() && !batchMode)
						trayIcon->showMessage(tr("Zakończono pobieranie napisów"), msg, QSystemTrayIcon::Information);
					else
#endif
						QMessageBox::information(0, tr("Zakończono pobieranie napisów"), msg);
				}
			}

			if(consoleMode)
			{
				qDebug(qPrintable(tr("\n * Podsumowanie:")));
				if(getThread.napiSuccess > 0)
					qDebug("   * %s", qPrintable(tr("Dopasowano napisy dla %1 %2.").arg(getThread.napiSuccess)
													.arg(tr((getThread.napiSuccess > 1) ? "plików" : "pliku"))));
				if(getThread.napiFail > 0)
					qDebug("   * %s", qPrintable(tr("Nie znaleziono napisów dla %1 %2!").arg(getThread.napiFail)
													.arg(tr((getThread.napiFail > 1) ? "plików" : "pliku"))));
			}
		}
		else
		{
			if(getThread.napiSuccess == 1)
			{
				QString msg = tr("Pobrano napisy dla pliku '%1'.").arg(QFileInfo(queue[0]).fileName());
				if(!quietMode)
				{
#ifndef Q_WS_MAC
					if(QSystemTrayIcon::supportsMessages() && !batchMode)
						trayIcon->showMessage(tr("Pobrano napisy"), msg, QSystemTrayIcon::Information);
					else
#endif
						QMessageBox::information(0, tr("Pobrano napisy"), msg);
				}

				if(consoleMode) qDebug(qPrintable(msg));
			}
			else
			{
				QString msg = tr("Nie znaleziono napisów dla:\n%1.").arg(QFileInfo(queue[0]).fileName());
				if(!quietMode)
			{
#ifndef Q_WS_MAC
				if(QSystemTrayIcon::supportsMessages() && !batchMode)
					trayIcon->showMessage(tr("Nie znaleziono napisów"), msg, QSystemTrayIcon::Information);
				else
#endif
						QMessageBox::information(0, tr("Nie znaleziono napisów"), msg);
				}
	
				if(consoleMode) qDebug(qPrintable(msg));
			}
		}
	}
	mutex.unlock();
	if(batchMode)
		qApp->quit();
}

void frmProgress::showOpenDialog()
{
	QStringList fileList;
	
	if(!openDialog)
	{
		if(!(openDialog = new QFileDialog(this)))
			return;

		connect(openDialog, SIGNAL(directoryEntered(const QString &)),
				this, SLOT(updatePreviousPath(const QString &)));

		openDialog->setAttribute(Qt::WA_QuitOnClose, false);
		openDialog->setWindowTitle(tr("Wybierz jeden lub więcej plików z filmami"));
		openDialog->setFilter(tr("Filmy (*.avi *.asf *.divx *.dat *.mkv *.mov *.mp4 *.mpeg *.mpg *.ogm "
								"*.rm *.rmvb *.wmv);; Wszystkie pliki (*.*)"));
		openDialog->setFileMode(QFileDialog::ExistingFiles);

		#ifdef Q_WS_MAC
			// Na Maku dodajemy folder '/Volumes' do paska bocznego
			// thx adrian5632
			QUrl volumes = QUrl::fromLocalFile("/Volumes");
			if ( (&volumes) && QDir().exists("/Volumes") && (!openDialog->sidebarUrls().contains(volumes)) )
			{
				QList<QUrl> urls = openDialog->sidebarUrls();
				urls.append(volumes);
				openDialog->setSidebarUrls(urls);
			}
			openDialog->setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
		#endif

		if (QFileInfo(GlobalConfig().previousDialogPath()).isDir())
			openDialog->setDirectory(GlobalConfig().previousDialogPath());
		else
			openDialog->setDirectory(QDir::currentPath());

	}

	if(openDialog->isVisible())
	{
		openDialog->raise();
		return;
	}

	// workaround dla compiza
	openDialog->move((QApplication::desktop()->width() - openDialog->width()) / 2, 
					(QApplication::desktop()->height() - openDialog->height()) / 2);

	if(openDialog->exec())
		fileList = openDialog->selectedFiles();

	delete openDialog;
	openDialog = 0;

	if(!fileList.isEmpty())
	{
		enqueueFiles(fileList);
		download();
	}
	else if(batchMode)
		qApp->quit();
}

void frmProgress::showScanDialog()
{
	if(!scan) scan = new frmScan(this);
	if(scan->isVisible())
	{
		scan->raise();
		return;
	}
	scan->exec();
	delete scan;
	scan = 0;
}

void frmProgress::showUploadDialog()
{
	if(!upload) upload = new frmUpload(this);
	if(upload->isVisible())
	{
		upload->raise();
		return;
	}
	upload->exec();
	delete upload;
	upload = 0;
}

void frmProgress::showCorrectDialog()
{
	if(!correct) correct = new frmCorrect(this);
	if(correct->isVisible())
	{
		correct->raise();
		return;
	}
	correct->exec();
	delete correct;
	correct = 0;
}

void frmProgress::showReportDialog()
{
	if(!report) report = new frmReport(this);
	if(report->isVisible())
	{
		report->raise();
		return;
	}
	report->exec();
	delete report;
	report = 0;
}

void frmProgress::showOptions()
{
	if(!options)
	{
		options = new frmOptions(this);
		options->readConfig();
	}

	if(options->isVisible())
	{
		options->raise();
		return;
	}

	if(options->exec() == QDialog::Accepted)
		options->writeConfig();

	delete options;
	options = 0;
}

void frmProgress::showCreateUser()
{
	if(!createUser) createUser = new frmCreateUser(this);
	if(createUser->isVisible())
	{
		createUser->raise();
		return;
	}
	createUser->exec();
	delete createUser;
	createUser = 0;
}

void frmProgress::showAbout()
{
	if(!about) about = new frmAbout(this);
	if(about->isVisible())
	{
		about->raise();
		return;
	}
	about->exec();
	delete about;
	about = 0;
}

void frmProgress::closeEvent(QCloseEvent *event)
{
	if(getThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"),
				tr("Czy chcesz przerwać pobieranie napisów?"),
				QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes )
		{
			event->ignore();
			return;
		}
		else
		{
			mutex.lock();
			showSummary = false;
			mutex.unlock();
			getThread.requestAbort();
			ui.lbAction->setText(tr("Kończenie zadań..."));
			ui.lbFileName->setText("...");
			qApp->processEvents();
			getThread.terminate();
			getThread.wait();
		}
	}
	event->accept();
}

void frmProgress::dragEnterEvent(QDragEnterEvent *event)
{
	QUrl url(event->mimeData()->urls().at(0));
	QFileInfo fi(url.toLocalFile());
	if(fi.exists() && fi.isFile())
		event->acceptProposedAction();
}

void frmProgress::dropEvent(QDropEvent *event)
{
	QList<QUrl> urlList;
	QFileInfo info;

	urlList = event->mimeData()->urls();

	foreach(QUrl url, urlList)
	{
		info.setFile(url.toLocalFile());
		if(!info.exists() || !info.isFile()) continue;
		enqueueFile(url.toLocalFile());
	}
}

void frmProgress::quit()
{
	if(isVisible() && !close()) return;
	qApp->quit();
}

void frmProgress::updatePreviousPath(const QString & path)
{
	GlobalConfig().setPreviousDialogPath(path);
}

void frmProgress::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger)
#ifndef Q_WS_MAC
		showOpenDialog()
#endif
	;
}

void GetThread::run()
{
	abort = false;
	if(queue.size() <= 0) return;

	QString windowTitle, md5;
	napiSuccess = napiFail = 0;
	gotList.clear();

	QNapiProjektEngine *napi;

	emit progressChange(0, queue.size(), 0.0f);

	for(int i = 0; i < queue.size(); i++)
	{
		napi = new QNapiProjektEngine(queue[i]);
		if(!napi) continue;

		if(verboseMode)
		{
			qDebug(" * %s '%s'", qPrintable(tr("Pobieranie napisów dla pliku")),
								qPrintable(QFileInfo(queue[i]).fileName()));
		}

		QFileInfo fi(queue[i]);
		emit fileNameChange(fi.fileName());

		emit progressChange(i, queue.size(), 0.33f);
		emit actionChange(tr("Obliczanie sumy kontrolnej pliku..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("obliczanie sumy kontrolnej")));

		napi->checksum();
		if(abort)
		{
			delete napi;
			return;
		}

		emit progressChange(i, queue.size(), 0.5f);
		emit actionChange(tr("Pobieranie napisów dla pliku..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("pobieranie napisów z serwera")));

		// pobieranie
//		if(!napiDownload(md5, tmpZip, GlobalConfig().language(),
//							GlobalConfig().nick(), GlobalConfig().pass()))
		if(!napi->tryDownload())
		{
			if(verboseMode) qDebug("   ! %s", qPrintable(tr("nie znaleziono napisów")));

			if(abort)
			{
				delete napi;
				return;
			}

			++napiFail;
			continue;
		}

		if(abort)
		{
			delete napi;
			return;
		}


		emit progressChange(i, queue.size(), 0.7f);
		emit actionChange(tr("Dopasowywanie napisów..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("dopasowywanie napisów")));

		// dopasowywanie
		//if(!napiMatchSubtitles(md5, tmpZip, queue[i], GlobalConfig().noBackup(),
		//						GlobalConfig().tmpPath(), GlobalConfig().p7zipPath()))
		if(!napi->tryMatch())
		{
			if(verboseMode) qDebug("   ! %s", qPrintable(tr("nie dało się dopasować napisów")));
			if(abort)
			{
				delete napi;
				return;
			}

			++napiFail;
			continue;
		}

		if(abort)
		{
			delete napi;
			return;
		}

		++napiSuccess;
		gotList << queue[i];

		if(GlobalConfig().changeEncoding())
		{
			emit progressChange(i, queue.size(), 0.85);
			emit actionChange(tr("Zmiana kodowania napisów..."));
			if(verboseMode) qDebug("   * %s...", qPrintable(tr("zmiana kodowania")));

			// Jesli automatycznie nie uda mu sie wykryc kodowania, to jako kodowania
			// zrodlowego uzywa kodowania wybranego przez uzytkownika
			if (!GlobalConfig().autoDetectEncoding()
				|| !napi->ppChangeSubtitlesEncoding(GlobalConfig().encodingTo()))
			{
				napi->ppChangeSubtitlesEncoding(GlobalConfig().encodingFrom(),
																GlobalConfig().encodingTo());
			}

			if(abort)
			{
				delete napi;
				return;
			}
		}

		emit progressChange(i, queue.size(), 1);

		delete napi;
	}

	emit progressChange(queue.size() - 1, queue.size(), 1);
}

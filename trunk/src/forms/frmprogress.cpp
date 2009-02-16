/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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
#include "qnapiapp.h"

frmProgress::frmProgress(QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	qRegisterMetaType<QNapiSubtitleInfoList>("QNapiSubtitleInfoList");

	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setBatchMode(false);

	connect(&getThread, SIGNAL(fileNameChange(const QString &)),
			ui.lbFileName, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(actionChange(const QString &)),
			ui.lbAction, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(progressChange(int, int, float)),
			this, SLOT(updateProgress(int, int, float)));
	connect(&getThread, SIGNAL(selectSubtitles(QString, QNapiSubtitleInfoList)),
			this, SLOT(selectSubtitles(QString, QNapiSubtitleInfoList)));
	connect(this, SIGNAL(subtitlesSelected(int)),
			&getThread, SLOT(subtitlesSelected(int)));
	connect(&getThread, SIGNAL(finished()),
			this, SLOT(downloadFinished()));
}

void frmProgress::receiveRequest(const QString & request)
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
	if(!QNapi::checkP7ZipPath())
	{
		QMessageBox::warning(0, tr("Brak programu p7zip!"),
								tr("Ścieżka do programu p7zip jest nieprawidłowa!"));
		return false;
	}

	if(!QNapi::checkTmpPath())
	{
		QMessageBox::warning(0, tr("Nieprawidłowy katalog tymczasowy!"),
								tr("Nie można pisać do katalogu tymczasowego! Sprawdź swoje ustawienia."));
		return false;
	}

	if(getThread.queue.isEmpty())
	{
		QMessageBox::warning(0, tr("Brak plików!"),
								tr("Nie wskazano filmów do pobrania napisów!"));
		return false;
	}

	if(!isVisible())
	{
		move((QApplication::desktop()->width() - width()) / 2, 
			(QApplication::desktop()->height() - height()) / 2);
		show();
	}

	showSummary = true;
	closeRequested = false;
	ui.pbCancel->setEnabled(true);

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

void frmProgress::selectSubtitles(QString fileName, QNapiSubtitleInfoList subtitles)
{
	frmListSubtitles frmSelect;

	frmSelect.setFileName(fileName);
	frmSelect.setSubtitlesList(subtitles);

	int selIdx;

	if(frmSelect.exec() == QDialog::Accepted)
	{
		selIdx = frmSelect.getSelectedIndex();
	}
	else
	{
		selIdx = -1;
	}

	emit subtitlesSelected(selIdx);
}

void frmProgress::downloadFinished()
{
	hide();

	QStringList queue = getThread.queue;
	getThread.queue.clear();

	mutex.lock();
	if(showSummary)
	{
		if(!getThread.criticalMessage.isEmpty())
		{
			QMessageBox::critical(0, tr("Błąd krytyczny!"), getThread.criticalMessage);
		}
		else if(queue.size() > 1)
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
					summary.setSuccessList(getThread.gotList);
					summary.setFailedList(getThread.failedList);
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
					((QNapiApp*)qApp)->showTrayMessage(tr("Zakończono pobieranie napisów"), msg);
				else
#endif
					QMessageBox::information(0, tr("Zakończono pobieranie napisów"), msg);
			}
		}
		else
		{
			if(getThread.napiSuccess == 1)
			{
				QString msg = tr("Pobrano napisy dla pliku '%1'.").arg(QFileInfo(queue[0]).fileName());
#ifndef Q_WS_MAC
				if(QSystemTrayIcon::supportsMessages() && !batchMode)
					((QNapiApp*)qApp)->showTrayMessage(tr("Pobrano napisy"), msg);
				else
#endif
					QMessageBox::information(0, tr("Pobrano napisy"), msg);
			}
			else
			{
				QString msg = tr("Nie pobrano napisów dla:\n%1.").arg(QFileInfo(queue[0]).fileName());
#ifndef Q_WS_MAC
				if(QSystemTrayIcon::supportsMessages() && !batchMode)
					((QNapiApp*)qApp)->showTrayMessage(tr("Nie pobrano napisów"), msg);
				else
#endif
					QMessageBox::information(0, tr("Nie pobrano napisów!"), msg);
			}
		}
	}

	if(closeRequested)
		close();

	mutex.unlock();
	if(batchMode)
		qApp->quit();
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
			getThread.requestAbort();
			ui.lbAction->setText(tr("Kończenie zadań..."));
			ui.lbFileName->setText("...");
			ui.pbCancel->setEnabled(false);
			qApp->processEvents();
			closeRequested = true;
			mutex.unlock();
			event->ignore();
		}
	}
	else
	{
		event->accept();
	}
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

void GetThread::subtitlesSelected(int idx)
{
	selIdx = idx;
	waitForDlg.unlock();
}

#define ABORT_POINT {if(abort){delete napi;return;}}

void GetThread::run()
{
	abort = false;
	criticalMessage.clear();
	if(queue.size() <= 0) return;

	QString windowTitle, md5;
	napiSuccess = napiFail = 0;
	gotList.clear();
	failedList.clear();

	QNapi *napi = new QNapi();

	if(!engines.isEmpty())
	{
		napi->addEngines(engines);
	}
	else if(!napi->addEngines(GlobalConfig().enginesList()))
	{
		emit criticalError(tr("Błąd: ") + napi->error());
		delete napi;
		return;
	}

	emit progressChange(0, queue.size(), 0.0f);

	QString language = !lang.isEmpty() ? lang : GlobalConfig().language();

	for(int i = 0; i < queue.size(); i++)
	{
		QFileInfo fi(queue[i]);
		emit fileNameChange(fi.fileName());

		napi->setMoviePath(queue[i]);

		emit progressChange(i, queue.size(), 0.1);
		emit actionChange(tr("Sprawdzanie uprawnień do katalogu z filmem..."));

		if(!napi->checkWritePermissions())
		{
			emit criticalError(tr("Brak uprawnień zapisu do katalogu '%1'!").arg(QFileInfo(queue[i]).path()));
			delete napi;
			return;
		}

		emit progressChange(i, queue.size(), 0.3);
		emit actionChange(tr("Obliczanie sumy kontrolnej pliku..."));

		napi->checksum();

		ABORT_POINT

		bool found = false;
		SearchPolicy sp = GlobalConfig().searchPolicy();		
		
		foreach(QString e, napi->listLoadedEngines())
		{
			emit progressChange(i, queue.size(), 0.4);
			emit actionChange(tr("Szukanie napisów (%1)...").arg(e));
			found = napi->lookForSubtitles(language, e) || found;

			if(sp == SP_BREAK_IF_FOUND)
				break;

			ABORT_POINT
		}

		if(!found)
		{
			++napiFail;
			failedList << queue[i];
			continue;
		}

		// jesli mozna i potrzeba, listujemy dostepne napisy
		if(napi->needToShowList())
		{
			emit selectSubtitles(	QFileInfo(queue[i]).fileName(),
									napi->listSubtitles());

			waitForDlg.lock();
			waitForDlg.lock();
			waitForDlg.unlock();
		}
		else
		{
			selIdx = napi->bestIdx();
		}

		ABORT_POINT

		if(selIdx == -1)
		{
			++napiFail;
			failedList << queue[i];
			continue;
		}

		emit progressChange(i, queue.size(), 0.5);
		emit actionChange(tr("Pobieranie napisów dla pliku..."));

		if(!napi->download(selIdx))
		{
			if(abort)
			{
				delete napi;
				return;
			}

			++napiFail;
			failedList << queue[i];
			continue;
		}

		ABORT_POINT

		emit progressChange(i, queue.size(), 0.6);
		emit actionChange(tr("Rozpakowywanie napisów..."));

		if(!napi->unpack())
		{
			++napiFail;
			failedList << queue[i];

			continue;
		}

		emit progressChange(i, queue.size(), 0.75);
		emit actionChange(tr("Dopasowywanie napisów..."));

		if(!napi->match())
		{
			ABORT_POINT

			++napiFail;
			failedList << queue[i];

			emit criticalError(tr("Nie udało się dopasować napisów!!"));

			delete napi;
			return;
		}

		++napiSuccess;
		gotList << queue[i];

		if(napi->ppEnabled())
		{
			emit progressChange(i, queue.size(), 0.9);
			emit actionChange(tr("Przetwarzanie napisów..."));
			napi->pp();
		}

		napi->cleanup();

		emit progressChange(i, queue.size(), 1);
	}

	emit progressChange(queue.size() - 1, queue.size(), 1);

	delete napi;
}

/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmprogress.h"

frmProgress::frmProgress(QWidget * parent, Qt::WFlags f) : QWidget(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_DeleteOnClose, false);
	setAttribute(Qt::WA_QuitOnClose, false);

	setBatchMode(false);
	setQuietMode(false);
	setConsoleMode(false);

	connect(&getThread, SIGNAL(windowTitleChange(const QString &)), this, SLOT(setWindowTitle(const QString &)));
	connect(&getThread, SIGNAL(fileNameChange(const QString &)), ui.lbFileName, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(actionChange(const QString &)), ui.lbAction, SLOT(setText(const QString &)));
	connect(&getThread, SIGNAL(progressChange(int)), ui.pbProgress, SLOT(setValue(int)));
	connect(&getThread, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void frmProgress::enqueueFiles(const QStringList & fileList)
{
	QStringList taskFileList;
	for(int i = 0; i < fileList.size(); i++)
	{
		if(QFile::exists(fileList.at(i)))
			taskFileList += fileList.at(i);
	}

	getThread.queue = taskFileList;
}

bool frmProgress::download()
{
	if(getThread.queue.isEmpty())
	{
		if(!quietMode)
			QMessageBox::warning(0, tr("Brak plików!"),
				tr("Nie wskazano filmów do pobrania napisów!"));
		if(consoleMode)
			qDebug("%s", qPrintable(tr("Nie wskazano filmów do pobrania napisów!")));
		return false;
	}

	if(!quietMode)
	{
		move((QApplication::desktop()->width() - width()) / 2, 
			(QApplication::desktop()->height() - height()) / 2);
		show();
	}

	getThread.start();
	return true;
}

void frmProgress::downloadFile(QString fileName)
{
	QStringList list;
	list << fileName;
	enqueueFiles(list);
	download();
}


void frmProgress::showOpenDialog()
{
	QStringList fileList;
	QFileDialog openDialog;
	connect(&openDialog, SIGNAL(directoryEntered(const QString &)),
			this, SLOT(updatePreviousPath(const QString &)));

	openDialog.setAttribute(Qt::WA_QuitOnClose, false);
	openDialog.setWindowTitle(tr("Wybierz jeden lub więcej plików z filmami"));
	openDialog.setFilter(tr("Filmy (*.avi *.asf *.divx *.dat *.mkv *.mov *.mp4 *.mpeg *.mpg *.ogm "
							"*.rm *.rmvb *.wmv);; Wszystkie pliki (*.*)"));
	openDialog.setFileMode(QFileDialog::ExistingFiles);

#ifdef Q_WS_MAC
	// Na Maku dodajemy folder '/Volumes' do paska bocznego
	// thx adrian5632
	QUrl volumes = QUrl::fromLocalFile("/Volumes");
	if ( (&volumes) && QDir().exists("/Volumes") && (!openDialog.sidebarUrls().contains(volumes)) )
	{
		QList<QUrl> urls = openDialog.sidebarUrls();
		urls.append(volumes);
		openDialog.setSidebarUrls(urls);
	}
	openDialog.setAttribute(Qt::WA_MacBrushedMetal, true);
#endif

	if (QFileInfo(GlobalConfig().previousDialogPath()).isDir())
		openDialog.setDirectory(GlobalConfig().previousDialogPath());
	else
		openDialog.setDirectory(QDir::currentPath());

	// workaround dla compiza
	openDialog.move((QApplication::desktop()->width() - openDialog.width()) / 2, 
					(QApplication::desktop()->height() - openDialog.height()) / 2);

	if(openDialog.exec())
		fileList = openDialog.selectedFiles();

	if(!fileList.isEmpty())
	{
		enqueueFiles(fileList);
		download();
	}
	else if(batchMode)
		qApp->quit();
}

void frmProgress::downloadFinished()
{
	hide();

	if(getThread.queue.size() > 1)
	{
		if(!quietMode)
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
			QString msg = tr("Pobrano napisy dla pliku '%1'.").arg(QFileInfo(getThread.queue[0]).fileName());
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
			QString msg = tr("Nie znaleziono napisów dla '%1'.").arg(QFileInfo(getThread.queue[0]).fileName());
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

	if(batchMode) qApp->quit();
}

void frmProgress::closeEvent(QCloseEvent *event)
{
	if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać pobieranie napisów?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
	{
		if(batchMode)
		{
			if(getThread.isRunning())
				getThread.requestAbort();

			ui.lbAction->setText(tr("Kończenie zadań..."));
			ui.lbFileName->setText("...");
			qApp->processEvents();
			getThread.terminate();
			getThread.wait();
			qApp->quit();
		}
		else
			hide();

		event->accept();
	}
	else
		event->ignore();
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

	aboutAction = new QAction(tr("O programie"), this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

	quitAction = new QAction(tr("Zakończ"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

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

void frmProgress::showOptions()
{
	frmOptions *options = new frmOptions(this);
	options->readConfig();
	if(options->exec() == QDialog::Accepted)
		options->writeConfig();
	delete options;
}

void frmProgress::showAbout()
{
	frmAbout *about = new frmAbout(this);
	about->exec();
	delete about;
}

void frmProgress::showScanDialog()
{
	frmScan *scan = new frmScan(this);
	scan->exec();
	delete scan;
}

void frmProgress::showUploadDialog()
{
	frmUpload *upload = new frmUpload(this);
	upload->exec();
	delete upload;
}

void frmProgress::showCorrectDialog()
{
	frmCorrect *correct = new frmCorrect(this);
	correct->exec();
	delete correct;
}

void frmProgress::showReportDialog()
{
	frmReport *report = new frmReport(this);
	report->exec();
	delete report;
}

void frmProgress::updatePreviousPath(const QString & path)
{
	GlobalConfig().setPreviousDialogPath(path);
}

void frmProgress::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
#ifndef Q_WS_MAC
	if(reason == QSystemTrayIcon::Trigger)
		showOpenDialog();
#endif
}

void GetThread::run()
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
		if(verboseMode)
		{
			qDebug(" * %s '%s'", qPrintable(tr("Pobieranie napisów dla pliku")),
								qPrintable(QFileInfo(queue[i]).fileName()));
		}

		windowTitle = (size > 1)
						? QString(tr("QNapi - pobieranie napisów (%1/%2)")).arg(i + 1).arg(size)
						: QString(tr("QNapi - pobieranie napisów..."));

		emit windowTitleChange(windowTitle);

		QFileInfo fi(queue[i]);
		emit fileNameChange(fi.fileName());

		emit actionChange(tr("Obliczanie sumy kontrolnej pliku..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("obliczanie sumy kontrolnej")));

		md5 = napiFileMd5Sum(queue[i], NAPI_10MB);
		if(abort) return;

		emit progressChange((int)ceil(step * i + step / 3));
		emit actionChange(tr("Pobieranie napisów dla pliku..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("pobieranie napisów z serwera")));

		// pobieranie
		if(!napiDownload(md5, tmpZip, GlobalConfig().language(),
							GlobalConfig().nick(), GlobalConfig().pass()))
		{
			if(verboseMode) qDebug("   ! %s", qPrintable(tr("nie znaleziono napisów")));

			if(abort) return;
			++napiFail;
			continue;
		}

		if(abort) return;

		emit progressChange((int)ceil(step * i + 2 * step / 3));
		emit actionChange(tr("Dopasowywanie napisów..."));
		if(verboseMode) qDebug("   * %s...", qPrintable(tr("dopasowywanie napisów")));

		// dopasowywanie
		if(!napiMatchSubtitles(md5, tmpZip, queue[i], GlobalConfig().noBackup(),
								GlobalConfig().tmpPath(), GlobalConfig().p7zipPath()))
		{
			if(verboseMode) qDebug("   ! %s", qPrintable(tr("nie dało się dopasować napisów")));
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
			if(verboseMode) qDebug("   * %s...", qPrintable(tr("zmiana kodowania")));

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

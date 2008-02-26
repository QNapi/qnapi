/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmupload.h"

frmUpload::frmUpload(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, true);
#endif
	setAttribute(Qt::WA_QuitOnClose, false);
	
	connect(ui.pbSelectDirectory, SIGNAL(clicked()), this, SLOT(selectDirectory()));
	connect(ui.leSelectDirectory, SIGNAL(textChanged(QString)), this, SLOT(leSelectDirectoryChanged()));
	connect(ui.pbScan, SIGNAL(clicked()), this, SLOT(pbScanClicked()));
	connect(&scanThread, SIGNAL(scanFinished(bool)), this, SLOT(scanFinished(bool)));
	connect(&scanThread, SIGNAL(folderChange(QString)), this, SLOT(folderChange(QString)));
	connect(ui.pbUpload, SIGNAL(clicked()), this, SLOT(pbUploadClicked()));
	connect(&uploadThread, SIGNAL(uploadFinished(bool)), this, SLOT(uploadFinished(bool)));
	connect(&uploadThread, SIGNAL(progressChange(int)), ui.pbProgress, SLOT(setValue(int)));
	connect(&uploadThread, SIGNAL(fileNameChange(QString)), this, SLOT(fileNameChange(QString)));
	connect(&uploadThread, SIGNAL(checkingUserPass()), this, SLOT(checkingUserPass()));
	connect(&uploadThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));
	
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

	if(uploadThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać wysyłanie napisów?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			uploadThread.requestAbort();
			ui.lbAction->setText(tr("Kończenie zadań..."));
			qApp->processEvents();
			uploadThread.terminate();
			uploadThread.wait();
			event->accept();
		}
		else
		{
			event->ignore();
		}
		return;
	}

	event->accept();
}

void frmUpload::selectDirectory()
{
	QString propDir = QFileInfo(ui.leSelectDirectory->text()).path();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Wskaż katalog do skanowania"),
					QDir().exists(propDir) ? propDir  : QDir::home().path(),
					QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
		ui.lbAction->setText(tr("Skanowanie folderów..."));
		ui.lbFoldersCount->setEnabled(false);
		ui.lbFoldersCount->setText(tr("Folderów: <b>0</b>"));
		ui.lbMoviesCount->setEnabled(false);
		ui.lbMoviesCount->setText(tr("Filmów: <b>0</b>"));
		ui.lbSubtitlesCount->setEnabled(false);
		ui.lbSubtitlesCount->setText(tr("Napisów: <b>0</b>"));
		ui.pbProgress->setEnabled(false);
		ui.pbUpload->setEnabled(false);

		scanThread.setSearchPath(ui.leSelectDirectory->text());
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
	ui.pbScan->setText(tr("Skanuj"));
	ui.lbAction->setText("");
	
	if(result)
	{
		if(scanThread.fileList.size() == 0)
		{
			QMessageBox::warning(this, tr("Nie znaleziono napisów"),
								tr("W wybranym folderze nie znaleziono żadnych napisów!"));
		}
		else
		{
			ui.lbAction->setText(tr("Teraz możesz dodać napisy do bazy NAPI."));
			ui.lbFoldersCount->setEnabled(true);
			ui.lbFoldersCount->setText(tr("Folderów: ") + "<b>"
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
		
		uploadThread.terminate();
		uploadThread.wait();
		
		ui.pbUpload->setEnabled(true);
		uploadFinished(true);
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

void frmUpload::uploadFinished(bool interrupt)
{
	ui.lbSelectDirectory->setEnabled(true);
	ui.leSelectDirectory->setEnabled(true);
	ui.pbSelectDirectory->setEnabled(true);
	ui.pbScan->setEnabled(true);
	ui.pbUpload->setText(tr("Wyślij do bazy NAPI"));
	ui.pbProgress->setValue(0);

	if(!interrupt)
	{
		ui.lbAction->setText(tr("Napisy wysłano."));
		
		QString msg = tr("Wysłano napisów: %1\n").arg(uploadThread.added_new+uploadThread.added_ok)
				+ tr("W tym zupełnie nowych: %1\n").arg(uploadThread.added_new)
				+ tr("Nie udało się wysłać: %1\n").arg(uploadThread.failed);

		QMessageBox::information(this, tr("Rezultat wysyłania"), msg);
	}
	else
		ui.lbAction->setText(tr("Przerwano."));
}

void frmUpload::invalidUserPass()
{
	QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void ScanThread::run()
{
	abort = false;
	folders = movies = subtitles = 0;
	fileList.clear();
	searchFilters.clear();
	searchFilters << "*.avi" << "*.asf" << "*.divx" << "*.dat" << "*.mkv" << "*.mov" << "*.mp4"
					<< "*.mpeg" << "*.mpg" << "*.ogm" << "*.rm" << "*.rmvb" << "*.wmv";

	emit scanFinished(doScan(searchPath));
}

bool ScanThread::doScan(const QString & path)
{
	QString myPath = QFileInfo(path).absoluteFilePath();
	if(!QDir().exists(myPath))
		return false;

	emit folderChange(myPath);
	++folders;

	QFileInfoList list = QDir(myPath).entryInfoList(searchFilters, QDir::AllDirs | QDir::Files
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

	emit checkingUserPass();

	if(!napiCheckUser(GlobalConfig().nick(), GlobalConfig().pass()))
	{
		emit invalidUserPass();
		emit uploadFinished(true);
		return;
	}
	
	int size = movieList.size();
	
	for(int i = 0; i < size; i++)
	{
		QFileInfo fi(movieList[i]);
		
		emit fileNameChange(fi.fileName());
		emit progressChange(i * 100 / size);
		
		switch( napiUploadSubtitles(movieList[i], fi.path() + "/" + fi.completeBaseName() + ".txt",
									GlobalConfig().language(), GlobalConfig().nick(),
									GlobalConfig().pass(), false, "", GlobalConfig().tmpPath(),
									GlobalConfig().p7zipPath()) )
		{
			case NAPI_ADDED_NEW: ++added_new; break;
			case NAPI_FAIL: ++failed; break;
			default: ++added_ok; break;
		}
		
		if(abort) break;
	}
	
	emit progressChange(100);
	emit uploadFinished();
}

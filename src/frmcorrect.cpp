/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmcorrect.h"

frmCorrect::frmCorrect(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
	setAttribute(Qt::WA_QuitOnClose, false);
	
	connect(ui.pbMovieSelect, SIGNAL(clicked()), this, SLOT(selectMovie()));
	connect(ui.pbSubtitlesSelect, SIGNAL(clicked()), this, SLOT(selectSubtitles()));
	connect(ui.leMovieSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
	connect(ui.leSubtitlesSelect, SIGNAL(textChanged(QString)), this, SLOT(checkPostEnable()));
	connect(ui.teComment, SIGNAL(textChanged()), this, SLOT(checkPostEnable()));
	connect(ui.pbPost, SIGNAL(clicked()), this, SLOT(pbPostClicked()));
	connect(&postThread, SIGNAL(postFinished()), this, SLOT(postFinished()));
	connect(&postThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmCorrect::closeEvent(QCloseEvent *event)
{
	if(postThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać wysyłanie poprawki?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			ui.lbAction->setText(tr("Kończenie zadań..."));
			qApp->processEvents();
			postThread.terminate();
			postThread.wait();
		}
		else
		{
			event->ignore();
			return;
		}
	}

	event->accept();
}

void frmCorrect::selectMovie()
{
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("Wskaż plik z filmem"), GlobalConfig().previousDialogPath(),
							tr("Filmy (*.avi *.asf *.divx *.dat *.mkv *.mov *.mp4 *.mpeg *.mpg "
								"*.ogm *.rm *.rmvb *.wmv);; Wszystkie pliki (*.*)"));

	if(!fileName.isEmpty() && QFile::exists(fileName))
		ui.leMovieSelect->setText(fileName);
}

void frmCorrect::selectSubtitles()
{
	QString path = QFileInfo(ui.leMovieSelect->text()).path();
	if(!QDir(path).exists())
		path = GlobalConfig().previousDialogPath();
	
	QString fileName = QFileDialog::getOpenFileName(this, tr("Wskaż plik z napisami"),
													path,
													tr("Napisy (*.txt);; Wszystkie pliki (*.*)"));

	if(!fileName.isEmpty() && QFile::exists(fileName))
		ui.leSubtitlesSelect->setText(fileName);
}

void frmCorrect::checkPostEnable()
{
	ui.pbPost->setEnabled(
			QFile::exists(ui.leMovieSelect->text()) &&
			QFile::exists(ui.leSubtitlesSelect->text()) &&
			!ui.teComment->toPlainText().isEmpty()
		);
	ui.lbAction->setText(ui.pbPost->isEnabled()
							? tr("Teraz możesz wysłać poprawione napisy.")
							: tr("Wypełnij wszystkie pola, aby wysłać poprawkę"));
}

void frmCorrect::pbPostClicked()
{
	if(!postThread.isRunning())
	{
		ui.leMovieSelect->setEnabled(false);
		ui.pbMovieSelect->setEnabled(false);
		ui.leSubtitlesSelect->setEnabled(false);
		ui.pbSubtitlesSelect->setEnabled(false);
		ui.teComment->setEnabled(false);
		ui.pbPost->setText(tr("Zatrzymaj"));
		ui.lbAction->setText(tr("Wysyłanie poprawki do serwera NAPI..."));
		
		postThread.setPostParams(ui.leMovieSelect->text(),
									ui.leSubtitlesSelect->text(),
									ui.teComment->toPlainText(),
									(ui.cbLanguage->currentIndex() == 0) ? "PL" : "ENG");
		postThread.start();
	}
	else
	{
		ui.lbAction->setText(tr("Przerywanie wysyłania..."));
		ui.pbPost->setEnabled(true);
		qApp->processEvents();
		
		postThread.terminate();
		postThread.wait();
		ui.pbPost->setEnabled(true);
		postFinished(true);
	}
}

void frmCorrect::postFinished(bool interrupted)
{
	ui.leMovieSelect->setEnabled(true);
	ui.pbMovieSelect->setEnabled(true);
	ui.leSubtitlesSelect->setEnabled(true);
	ui.pbSubtitlesSelect->setEnabled(true);
	ui.teComment->setEnabled(true);
	ui.pbPost->setText(tr("Wyślij"));
	
	if(interrupted)
	{
		ui.lbAction->setText(tr("Przerwano wysyłanie poprawki."));
	}
	else
	{
		switch(postThread.taskResult)
		{
			case NAPI_FAIL: ui.lbAction->setText(tr("Błąd podczas wysyłania poprawki.")); break;
			default: ui.lbAction->setText(tr("Poprawka wysłana."));
		}
	}
}

void frmCorrect::invalidUserPass()
{
	QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void PostThread::run()
{
	if(!napiCheckUser(GlobalConfig().nick(), GlobalConfig().pass()))
	{
		emit invalidUserPass();
		emit postFinished();
		return;
	}

	taskResult = napiUploadSubtitles(movie, subtitles, language, GlobalConfig().nick(),
										GlobalConfig().pass(), true, comment,
										GlobalConfig().tmpPath(), GlobalConfig().p7zipPath());

	emit postFinished();
}

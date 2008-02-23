/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmreport.h"

frmReport::frmReport(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, true);
#endif
	setAttribute(Qt::WA_QuitOnClose, false);
	
	connect(ui.pbMovieSelect, SIGNAL(clicked()), this, SLOT(selectMovie()));
	connect(ui.leMovieSelect, SIGNAL(textChanged(QString)), this, SLOT(checkReportEnable()));
	connect(ui.cbProblem, SIGNAL(currentIndexChanged(int)), this, SLOT(cbProblemChanged()));
	connect(ui.leProblem, SIGNAL(textChanged(QString)), this, SLOT(checkReportEnable()));
	connect(ui.pbReport, SIGNAL(clicked()), this, SLOT(pbReportClicked()));
	connect(&reportThread, SIGNAL(reportFinished()), this, SLOT(reportFinished()));
	connect(&reportThread, SIGNAL(serverMessage(QString)), this, SLOT(serverMessage(QString)));
	connect(&reportThread, SIGNAL(invalidUserPass()), this, SLOT(invalidUserPass()));
}

void frmReport::closeEvent(QCloseEvent *event)
{
	if(reportThread.isRunning())
	{
		if( QMessageBox::question(this, tr("QNapi"), tr("Czy chcesz przerwać wysyłanie raportu?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
		{
			ui.lbAction->setText(tr("Kończenie zadań..."));
			qApp->processEvents();
			reportThread.terminate();
			reportThread.wait();
		}
		else
		{
			event->ignore();
			return;
		}
	}
	
	event->accept();
}

void frmReport::selectMovie()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Wskaż plik z filmem"),
													QDir::currentPath(),
													tr("Filmy (*.avi *.asf *.divx *.mkv *.mp4"
													" *.mpeg *.mpg *.ogm *.rm *.rmvb *.wmv);;"
													"Wszystkie pliki (*.*)"));

	if(!fileName.isEmpty() && QFile::exists(fileName))
		ui.leMovieSelect->setText(fileName);
}

void frmReport::checkReportEnable()
{
	ui.pbReport->setEnabled(
			QFile::exists(ui.leMovieSelect->text()) &&
			((ui.cbProblem->currentIndex() == 4)
				? !ui.leProblem->text().isEmpty()
				: true)
		);
	ui.lbAction->setText(ui.pbReport->isEnabled()
							? tr("Teraz możesz wysłać raport.")
							: tr("Wskaż plik z filmem oraz opisz problem."));
}

void frmReport::cbProblemChanged()
{
	ui.leProblem->setEnabled(ui.cbProblem->currentIndex() == 4);
	checkReportEnable();
}

void frmReport::pbReportClicked()
{
	if(!reportThread.isRunning())
	{
		ui.leMovieSelect->setEnabled(false);
		ui.pbMovieSelect->setEnabled(false);
		ui.cbLanguage->setEnabled(false);
		ui.cbProblem->setEnabled(false);
		ui.leProblem->setEnabled(false);
		ui.pbReport->setText(tr("Zatrzymaj"));
		ui.lbAction->setText(tr("Wysyłanie raportu do serwera NAPI..."));
		
		reportThread.setReportParams(ui.leMovieSelect->text(),
									(ui.cbLanguage->currentIndex() == 0) ? "PL" : "ENG",
									(ui.cbProblem->currentIndex() < 4
										? ui.cbProblem->currentText()
										: ui.leProblem->text()) );
		reportThread.start();
	}
	else
	{
		ui.lbAction->setText(tr("Przerywanie wysyłania..."));
		ui.pbReport->setEnabled(true);
		qApp->processEvents();
		
		reportThread.terminate();
		reportThread.wait();
		ui.pbReport->setEnabled(true);
		reportFinished(true);
	}
}

void frmReport::reportFinished(bool interrupted)
{
	ui.leMovieSelect->setEnabled(true);
	ui.pbMovieSelect->setEnabled(true);
	ui.cbLanguage->setEnabled(true);
	ui.cbProblem->setEnabled(true);
	ui.leProblem->setEnabled(true);
	cbProblemChanged();
	
	ui.pbReport->setText(tr("Wyślij"));
	
	if(interrupted)
	{
		ui.lbAction->setText(tr("Przerwano wysyłanie poprawki."));
	}
	else
	{
		switch(reportThread.taskResult)
		{
			case NAPI_NO_SUBTITLES:
				ui.lbAction->setText(tr("Brak napisów dla wskazanego pliku."));
			break;
			case NAPI_NOT_REPORTED:
				ui.lbAction->setText(tr("Błąd podczas wysyłania raportu."));
			break;
			default: ui.lbAction->setText(tr("Raport wysłany."));
		}
	}
}

void frmReport::serverMessage(QString msg)
{
	if(msg.indexOf("NPc0") == 0)
		msg = tr("Zgłoszono raport do serwera NAPI.");
	else
		msg = tr("Odpowiedź serwera: ") + tr(qPrintable(msg));
	
	QMessageBox::information(this, tr("Raport wysłany"), msg);
}

void frmReport::invalidUserPass()
{
	QMessageBox::information(this, tr("Błąd!"), QString(tr("Nazwa użytkownika lub hasło jest niepoprawne.")));
}

void ReportThread::run()
{
	if(!napiCheckUser(GlobalConfig().nick(), GlobalConfig().pass()))
	{
		emit invalidUserPass();
		emit reportFinished();
		return;
	}

	QString *response = new QString();
	taskResult = napiReportBad(movie, language, GlobalConfig().nick(),
								GlobalConfig().pass(), comment, response);
	if(taskResult == NAPI_REPORTED)
		emit serverMessage(*response);
	delete response;
	
	emit reportFinished();
}

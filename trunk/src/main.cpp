/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QStringList>
#include <QTextCodec>
#include "frmprogress.h"
#include "version.h"
#include "napi.h"
#include "qnapiconfig.h"
#include "qnapiapp.h"

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("QNapi");
	QNapiApp app(argc, argv);

	app.setQuitOnLastWindowClosed(false);
	QTextCodec::setCodecForTr(QTextCodec::codecForName ("UTF-8"));

	QStringList args = app.arguments();

	if(args.size() > 0 )
		args.removeAt(0);

	if(!app.isInstanceAllowed())
	{
		for(int i = 0; i < args.size(); i++)
		{
			app.sendRequest(args[i]);
		}
		return 1;
	}

	frmProgress form(0, 0);
	form.connect(&app, SIGNAL(request(QString)), SLOT(receiveRequest(QString)));

	// Przegladamy parametry i sprawdzamy w jakim trybie aplikacja jest odpalona
	#ifndef Q_WS_WIN // pod Windowsem i tak nie zadziala ;/
	for(int i=0; i < args.size(); i++)
	{
		if((args[i] == "--help") || (args[i] == "-h"))
		{
			qDebug("QNapi, wersja %s", QNAPI_VERSION);
			qDebug("QNapi jest wolnym odpowiednikiem programu NAPI-PROJEKT.");
			qDebug("Program rozprowadzany jest na licencji GNU General Public License.\n");
			qDebug("Użycie programu: %s [opcje] [lista plików]",
					qPrintable(QFileInfo(app.arguments()[0]).fileName()));
			qDebug("Dostępne opcje:");
			qDebug("    -c, --console    pobieranie napisów z konsoli");
			qDebug("    -q, --quiet      pobiera napisy nie wypisując żadnych komunikatów");
			qDebug("                     ani nie pokazując żadnych okien");
			qDebug("    -h, --help       tekst pomocy, który właśnie czytasz\n");

			return 0;
		}
		else if((args[i] == "--quiet") || (args[i] == "-q"))
		{
			form.setQuietMode(true);
			args.removeAt(i--);
			continue;
		}
		else if((args[i] == "--console") || (args[i] == "-c"))
		{
			form.setConsoleMode(true);
			args.removeAt(i--);
			continue;
		}
	}
	#endif

	// Jesli podano parametry, to znaczy ze sa to nazwy plikow, ktorym
	// QNapi powinien dopasowac napisy
	if((args.size() > 0) || form.isQuietMode())
	{
		form.enqueueFiles(args);
		form.setBatchMode(true);
		if(!form.download()) return 1;
	}
	else if(!form.isBatchMode()) // Normalne uruchomienie, ikona w trayu
	{
		if(GlobalConfig().firstRun())
		{
			if(QMessageBox::question(0, QObject::tr("Pierwsze uruchomienie"),
					QObject::tr("To jest pierwsze uruchomienie programu QNapi. Czy chcesz go "
					"teraz skonfigurować?"), QMessageBox::Yes | QMessageBox::No )
				== QMessageBox::Yes )
			{
				form.showOptions();
			}
		}

		// Jesli nie ma traya, od razu wyswietlamy okienko z wyborem pliku
		if(!QSystemTrayIcon::isSystemTrayAvailable())
		{
			form.setBatchMode(true);
			form.showOpenDialog();
		}
		else
			form.createTrayIcon();
	}

	if(!form.isBatchMode())
		QObject::connect(&app, SIGNAL(downloadFile(QString)), &form, SLOT(receiveRequest(QString)));

	return app.exec();
}

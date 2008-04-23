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

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QStringList>
#include <QTextCodec>
#include "frmprogress.h"
#include "version.h"
#include "qnapiconfig.h"
#include "qnapiapp.h"

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("QNapi");
	QNapiApp app(argc, argv);

	app.setQuitOnLastWindowClosed(false);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	QStringList args = app.arguments();

	if(args.size() > 0 )
		args.removeAt(0);

	if(!app.isInstanceAllowed())
	{
		for(int i = 0; i < args.size(); i++)
			if(QFileInfo(args[i]).isFile())
				app.sendRequest(args[i]);
		//system("pause");
		return 0;
	}

	frmProgress form(0, 0);
	form.connect(&app, SIGNAL(request(QString)), SLOT(receiveRequest(QString)));

	// Przegladamy parametry i sprawdzamy w jakim trybie aplikacja jest odpalona
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

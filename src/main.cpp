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
#include "version.h"
#include "qnapiconfig.h"
#include "qnapiapp.h"
#include "qnapicli.h"

int main(int argc, char *argv[])
{
	// Workarond: potrzebne, aby QEventLoop nie krzyczal, ze mu brakuje obiektu aplikacji
	QCoreApplication *a = new QCoreApplication(argc, argv);

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	QNapiCli cliApp(argc, argv);
	if(cliApp.analyze())
		return cliApp.exec();

	// Kasujemy tymczasowy obiekt aplikacji
	if(a) delete a;

	QNapiApp app(argc, argv);
	QNapiApp::setApplicationName("QNapi");

	app.setQuitOnLastWindowClosed(false);

	QStringList args = app.arguments();

	if(args.size() > 0 )
		args.removeAt(0);

	// Jesli uruchomiono aplikacje poprzez alt+f2 w KDE3 i przeciagnieto plik
	// to trzeba usunac 'file://' dodane przez okno uruchamiania
	for (int i = 0; i < args.size(); i++)
	{
		if(args[i].startsWith("file://"))
			args[i] = args[i].remove(0, 7);
	}
	// thx adrian5632

	if(!app.isInstanceAllowed())
	{
		for(int i = 0; i < args.size(); i++)
			if(QFileInfo(args[i]).isFile())
				app.sendRequest(args[i]);
		return 0;
	}

	if(GlobalConfig().firstRun())
	{
		if(QMessageBox::question(0, QObject::tr("Pierwsze uruchomienie"),
				QObject::tr("To jest pierwsze uruchomienie programu QNapi. Czy chcesz go "
				"teraz skonfigurować?"), QMessageBox::Yes | QMessageBox::No )
			== QMessageBox::Yes )
		{
			app.showOptions();
		}
	}

	// Jesli podano parametry, ustawiamy tzw. batch mode
	if(args.size() > 0)
	{
		app.progress->enqueueFiles(args);
		app.progress->setBatchMode(true);
		if(!app.progress->download()) return 1;
	}

	// Jesli nie dzialamy w trybie pobierania, mozemy ew. utworzyc ikone w tray-u
	// badz pokazac okno wyboru plikow z filmami
	if(!app.progress->isBatchMode())
	{
		// Jesli nie ma traya, od razu wyswietlamy okienko z wyborem pliku
		if(!QSystemTrayIcon::isSystemTrayAvailable())
		{
			app.progress->setBatchMode(true);
			app.showOpenDialog();
		}
		else // Jesli ikona w tray-u jest obsligiwana, tworzymy ja
		{
			app.createTrayIcon();
		}
	}

	return app.exec();
}

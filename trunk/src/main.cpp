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
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include "version.h"
#include "qnapiconfig.h"
#include "qnapiapp.h"
#include "qnapicli.h"

int main(int argc, char **argv)
{

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	QStringList args;
	for(int i = 1; i < argc; i++)
	{
		QString p = argv[i];

		if(p.startsWith("file://"))
			p = p.remove(0, 7);

		if((i == 1) && QFileInfo(p).isDir())
		{
			args << p;
			break;
		}

		if(QFileInfo(p).isFile())
			args << p;
	}

#ifndef Q_WS_WIN
	QNapiCli *cliApp = new QNapiCli(argc, argv);
	bool useGui = !cliApp->analyze();
#else
	bool useGui = true;
#endif


	QNapiApp app(argc, argv, true/*useGui*/, "QNapi");

	QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	QTranslator cuteTranslator;
	cuteTranslator.load("qt_" + QLocale::system().name(), resourceDir);
	app.installTranslator(&cuteTranslator);

#ifndef Q_WS_WIN
	if(!useGui)
	{
		int r = cliApp->exec();
		delete cliApp;
		return r;
	}
	else
	{
		delete cliApp;
	}
#endif

	app.setQuitOnLastWindowClosed(false);

	if(!app.isInstanceAllowed())
	{
		for(int i = 0; i < args.size(); i++)
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
			app.showSettings();
		}
	}
	
	// Jesli podano parametry, ustawiamy tzw. batch mode
	if(args.size() > 0)
	{
		if(QFileInfo(args.at(0)).isDir())
		{
			app.progress()->setBatchMode(true);
			app.showScanDialog(args.at(0));
		}
		else
		{
			app.progress()->enqueueFiles(args);
			app.progress()->setBatchMode(true);
			if(!app.progress()->download()) return 1;
		}
	}

	// Jesli nie dzialamy w trybie pobierania, mozemy ew. utworzyc ikone w tray-u
	// badz pokazac okno wyboru plikow z filmami
	if(!app.progress()->isBatchMode())
	{
		// Jesli nie ma traya, od razu wyswietlamy okienko z wyborem pliku
		if(!QSystemTrayIcon::isSystemTrayAvailable())
		{
			app.progress()->setBatchMode(true);
			if(!app.showOpenDialog())
				return 1;
		}
		else // Jesli ikona w tray-u jest obsligiwana, tworzymy ja
		{
			app.createTrayIcon();
		}
	}

	return app.exec();
}

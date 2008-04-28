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

#include "qnapicli.h"

bool QNapiCli::analyze()
{
	QString p;

	for(int i = 1; i < m_argc; i++)
	{
		p = m_argv[i];

		if((p == "--help") || (p == "-h"))
		{
			mode = CM_HELP;
			return true;
		}
		else if((p == "--console") || (p == "-c"))
		{
			if(mode == CM_UNSET)
				mode = CM_CONSOLE;
		}
		else if((p == "--quiet") || (p == "-q"))
		{
			mode = CM_QUIET;
		}
		else if(QFileInfo(p).isFile())
		{
			movieList << p;
		}
	}

	return (mode == CM_CONSOLE) || (mode == CM_QUIET);
}

int QNapiCli::exec()
{
	if(mode != CM_QUIET)
		printCli(QString("QNapi %1, (C) 2008 Krzemin, http://krzemin.iglu.cz\n").arg(QNAPI_VERSION));

	if(mode == CM_HELP)
	{
		printHelp();
		return 0;
	}

	if(movieList.empty())
	{
		printCli("Nie podano plikow z napisami!");
		return 1;
	}

	QNapiProjektEngine *napi;

	foreach(QString movie, movieList)
	{
		napi = new QNapiProjektEngine(movie);
		if(!napi) continue;

		printCli(QString(QString(" * Pobieranie napisow dla '%1'")).arg(QFileInfo(movie).fileName()));

		printCli(QString(QString("   obliczanie sumy kontrolnej...")));
		napi->checksum();

		printCli(QString(QString("   pobieranie napisow z serwera...")));
		if(!napi->tryDownload())
		{
			printCli(QString(QString("   ! nie znaleziono napisow")));
			delete napi;
			continue;
		}

		printCli(QString(QString("   dopasowywanie napisow...")));
		if(!napi->tryMatch())
		{
			printCli(QString(QString("   ! nie udalo sie dopasowac napisow")));
			delete napi;
			continue;
		}

		if(GlobalConfig().ppEnabled())
		{
			printCli(QString(QString("   post-przetwarzanie pobranych napisow...")));
			napi->doPostProcessing();
		}

		delete napi;
	}

	return 0;
}

void QNapiCli::printHelp()
{
	printCli(QString("QNapi jest programem do automatycznego pobierania i dopasowywania"));
	printCli(QString("napisow do filmow, korzystajacym z bazy programu NAPI-PROJEKT.\n"));
	printCli(QString("QNapi rozprowadzany jest na warunkach licencji GNU General Public License v2.\n"));
	printCli(QString("Uzycie programu: %1 [opcje] [lista plikow]").arg(m_argv[0]));
	printCli(QString("Dostepne opcje:"));
	printCli(QString("    -c, --console    pobieranie napisow z konsoli"));
	printCli(QString("    -q, --quiet      pobiera napisy nie wypisujac zadnych komunikatow"));
	printCli(QString("                     ani nie pokazujac zadnych okien"));
	printCli(QString("    -h, --help       tekst pomocy, ktory wlasnie czytasz\n"));
}

void QNapiCli::printCli(const QString & string)
{
	if(mode != CM_QUIET)
		qDebug(qPrintable(string));
}

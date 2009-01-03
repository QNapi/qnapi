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

#ifdef Q_WS_X11
	if((getenv("DISPLAY") == 0) && (mode == CM_UNSET))
		mode = CM_CONSOLE;
#endif

	return (mode == CM_CONSOLE) || (mode == CM_QUIET);
}

int QNapiCli::exec()
{
	if(mode != CM_QUIET)
		printCli(QString("QNapi %1, (C) 2008 Krzemin, http://krzemin.iglu.cz/qnapi\n").arg(QNAPI_VERSION));

	if(mode == CM_HELP)
	{
		printHelp();
		return 0;
	}

	if(movieList.empty())
	{
		printCli("Nie podano sciezki do pliku z filmem!");
		return 1;
	}

	if(!QNapi::checkP7ZipPath())
	{
		printCli("Sciezka do programu p7zip jest nieprawidlowa! Sprawdz swoje ustawienia.");
		return 2;
	}

	if(!QNapi::checkTmpPath())
	{
		printCli("Nie można pisac do katalogu tymczasowego! Sprawdz swoje ustawienia.");
		return 3;
	}

	QNapi *napi = new QNapi();
	
/*	if(!napi->addEngine("NapiProjekt"))
	{
		printCli("Błąd! Nie udało się zainicjalizować silnika NapiProjekt!");
		delete napi;
		return 1;
	}*/

	if(!napi->addEngine("OpenSubtitles"))
	{
		printCli("Błąd! Nie udało się zainicjalizować silnika NapiProjekt!");
		delete napi;
		return 1;
	}


	foreach(QString movie, movieList)
	{
		printCli(QString(QString(" * Pobieranie napisow dla '%1'")).arg(QFileInfo(movie).fileName()));

		napi->setMoviePath(movie);

		if(!napi->checkWritePermissions())
		{
			printCli(QString("   brak uprawnien zapisu do katalogu '%1'!").arg(QFileInfo(movie).path()));
			continue;
		}

		printCli(QString(QString("   obliczanie sum kontrolnych...")));
		napi->checksum();

		printCli(QString(QString("   szukanie napisów...")));
		if(!napi->lookForSubtitles("PL") || (napi->listSubtitles().size() == 0))
		{
			printCli(QString(QString("   nie znaleziono napisow!")));
			continue;
		}
		
		int selIdx = 0;

		// jesli mozna i potrzeba, listujemy dostepne napisy
		if((mode != CM_QUIET) && (napi->needToShowList()))
		{
//			if(GlobalConfig().downloadPolicy() == pokazuj zawsze)
//			{
	
			bool ok = false;
			
			printCli("   znalezione napisy:");			
			
			while(!ok)
			{
				int i = 1;

				QList<QNapiSubtitleInfo> list = napi->listSubtitles();

				printCli(QString("   0)\tnie pobieraj napisow dla tego filmu"));

				foreach(QNapiSubtitleInfo s, list)
				{
					QString resolution = "";
					
					if(s.resolution == SUBTITLE_GOOD)
						resolution = " (good)";
					else if(s.resolution == SUBTITLE_BAD)
						resolution = " (bad)";
					
					printCli(QString("   %1)\t%2 (%3) (%4) (%5)%6")
								.arg(i++)
								.arg(s.name)
								.arg(s.format)
								.arg(s.lang)
								.arg(s.engine)
								.arg(resolution));
				}
	
				std::cout << "   wybierz napisy do pobrania: ";
				char line[16];
				std::cin.getline(line, 16);
				
				selIdx = QString(line).toInt(&ok);
	
				if(!ok)
				{
					printCli("   wpisz liczbe!");
				}
				else if((selIdx > list.size()) || (selIdx < 0))
				{
					ok = false;
					printCli("   wpisz liczbe z listy!");
				}
			}
			
			--selIdx;
				
//			}
		}

		if(selIdx == -1) continue;

		printCli(QString(QString("   pobieranie napisow z serwera...")));
		if(!napi->download(selIdx))
		{
			printCli(QString(QString("   nie udało się pobrać napisów!")));
			continue;
		}

		printCli(QString(QString("   rozpakowywanie napisów...")));
		if(!napi->unpack())
		{
			printCli(QString(QString("   nie udało się poprawnie rozpakować napisów!")));
			continue;
		}


		printCli(QString(QString("   dopasowywanie napisow...")));
		if(!napi->match())
		{
			printCli(QString(QString("   nie udalo sie dopasowac napisow!")));
			delete napi;
			continue;
		}

		if(napi->ppEnabled())
		{
			printCli(QString(QString("   przetwarzanie pobranych napisow...")));
			napi->pp();
		}
		
		napi->cleanup();
	}

	delete napi;

	return 0;
}

void QNapiCli::printHelp()
{
	printCli(QString("QNapi jest programem do automatycznego pobierania i dopasowywania"));
	printCli(QString("napisow do filmow, korzystajacym z bazy programu NAPI-PROJEKT.\n"));
	printCli(QString("QNapi rozprowadzany jest na warunkach licencji GNU General Public License v2.\n"));
	printCli(QString("Uzycie programu: %1 [opcje] [lista plikow]").arg(m_argv[0]));
	printCli(QString("Dostepne opcje:"));
	printCli(QString("    -c, --console          pobieranie napisow z konsoli"));
	printCli(QString("    -l, --show-list        pokazuj liste napisow"));
	printCli(QString("    -d, --dont-show-list   nie pokazuj listy napisow"));
	printCli(QString("    -q, --quiet            pobiera napisy nie wypisujac zadnych komunikatow"));
	printCli(QString("                           ani nie pokazujac zadnych okien (implikuje -d)"));
	printCli(QString("    -h, --help             pokazuje tekst pomocy, ktory wlasnie czytasz\n"));
}

void QNapiCli::printCli(const QString & string)
{
	if(mode != CM_QUIET)
//		qDebug(qPrintable(string));
		std::cout << string.toStdString() << std::endl;
}

/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

bool QNapiCli::isCliCall(int argc, char **argv)
{
    QString p;

#ifdef Q_OS_WIN
    for(int i = 1; i < argc; i++)
    {
        p = argv[i];
        if((p == "--quiet") || (p == "-q"))
            return true;
    }
    return false;
#else
    for(int i = 1; i < argc; i++)
    {
        p = argv[i];

        if(p == "--options")
        {
            return false;
        }

        if( (p == "--console") || (p == "-c") ||
           (p == "--quiet") || (p == "-q") )
        {
            return true;
        }
        
        if( (p == "--help") || (p == "-h") ||
            (p == "--help-languages") || (p == "-hl"))
        {
            return true;
        }
    }

    #if defined(Q_OS_MAC)
        return false;
    #elif (defined(Q_OS_UNIX) || defined(Q_OS_LINUX))
        if(getenv("DISPLAY") == 0)
            return true;
        return false;
    #else
        return false;
    #endif
#endif
}

bool QNapiCli::analyze()
{
    QString p;
    QStringList args = arguments();

    for(int i = 1; i < args.size(); i++)
    {
        p = args[i];

        if((p == "--help") || (p == "-h"))
        {
            mode = CM_HELP;
            return true;
        }
        else if((p == "--help-languages") || (p == "-hl"))
        {
            mode = CM_HELP_LANGUAGES;
            return true;
        }
        else if((p == "--console") || (p == "-c"))
        {
            if(mode == CM_UNSET)
            {
#ifdef Q_OS_WIN
                mode = CM_QUIET;
                showPolicy = SLP_NEVER_SHOW;
#else
                mode = CM_CONSOLE;
#endif
            }
        }
        else if((p == "--quiet") || (p == "-q"))
        {
            mode = CM_QUIET;
            showPolicy = SLP_NEVER_SHOW;
        }
        else if((p == "--lang") || (p == "-l"))
        {
            ++i;
            if(i >= args.size())
                return false;

            p = args[i];

            lang = QNapiLanguage(p).toTwoLetter();
            if(lang.isEmpty())
            {
                printCli(QString("Niepoprawny kod jezykowy: %1").arg(p));
                return false;
            }
        }
        else if((p == "--lang-backup") || (p == "-lb"))
        {
            ++i;
            if(i >= args.size())
                return false;

            p = args[i];

            langBackup = QNapiLanguage(p).toTwoLetter();
            langBackupPassed = true;
        }
        else if((p == "--show-list") || (p == "-s"))
        {
            if(mode != CM_QUIET)
                showPolicy = SLP_SHOW;
        }
        else if((p == "--dont-show-list") || (p == "-d"))
        {
            showPolicy = SLP_NEVER_SHOW;
        }
        else if(QFileInfo(p).isFile())
        {
            movieList << p;
        }
    }

#if (defined(Q_OS_UNIX) || defined(Q_OS_LINUX))
    if((getenv("DISPLAY") == 0) && (mode == CM_UNSET))
        mode = CM_CONSOLE;
#endif

    return true;
}

int QNapiCli::exec()
{
    if(!analyze())
        return EC_CMD_LINE_ARG_PARSING_ERROR;

    if(mode == CM_UNSET)
        return 1;

    if(mode != CM_QUIET)
        printHeader();

    if(mode == CM_HELP_LANGUAGES)
    {
        printHelpLanguages();
        return EC_OK;
    }

    if((mode == CM_CONSOLE || mode == CM_QUIET) && (movieList.isEmpty())) {
        printCli("Nie znaleziono pliku!");
        return EC_MOVIE_FILE_NOT_FOUND;
    }

    if(mode == CM_HELP)
    {
        printHelp();
        return EC_OK;
    }

    if(!QNapi::checkP7ZipPath())
    {
        printCli("Sciezka do programu p7zip jest nieprawidlowa! Sprawdz swoje ustawienia.");
        return EC_P7ZIP_UNAVAILABLE;
    }

    if(!QNapi::checkTmpPath())
    {
        printCli("Nie można pisac do katalogu tymczasowego! Sprawdz swoje ustawienia.");
        return EC_CANNOT_WRITE_TMP_DIR;
    }

    
    if(!napi.addEngines(GlobalConfig().enginesList()))
    {
        printCli(QString("Blad: ") + napi.error());
        return EC_UNSUPPORTED_ENGINE;
    }

    if(lang.isEmpty())
        lang = GlobalConfig().language();

    if(!langBackupPassed)
        langBackup = GlobalConfig().languageBackup();

    foreach(QString movie, movieList)
    {
        printCli(QString(QString(" * Pobieranie napisow dla '%1'")).arg(QFileInfo(movie).fileName()));

        napi.setMoviePath(movie);

        if(!napi.checkWritePermissions())
        {
            printCli(QString("   Brak uprawnien zapisu do katalogu '%1'!").arg(QFileInfo(movie).path()));
            return EC_NO_WRITE_PERMISSIONS;
        }

        printCli(QString(QString("   Obliczanie sum kontrolnych...")));
        napi.checksum();

        bool found = false;
        SearchPolicy sp = GlobalConfig().searchPolicy();

        foreach(QString e, napi.listLoadedEngines())
        {
            printCli(QString(QString("   Szukanie napisow [%1] (%2)...").arg(lang, e)));
            found = napi.lookForSubtitles(lang, e) || found;

            if(sp == SP_BREAK_IF_FOUND)
                break;
        }

        if(!found && !langBackup.isEmpty()) {
            foreach(QString e, napi.listLoadedEngines())
            {
                printCli(QString(QString("   Szukanie napisow w jezyku zapasowym [%1] (%2)...").arg(langBackup, e)));
                found = napi.lookForSubtitles(langBackup, e) || found;

                if(sp == SP_BREAK_IF_FOUND)
                    break;
            }
        }

        if(!found)
        {
            printCli(QString(QString("   Nie znaleziono napisow!")));
            return EC_SUBTITLES_NOT_FOUND;
        }

        int selIdx = 0;

        bool showList = false;
        bool napiShowList = napi.needToShowList();

        if((mode != CM_QUIET) && (showPolicy != SLP_NEVER_SHOW))
            showList = napiShowList;
        else if(showPolicy == SLP_SHOW)
            showList = true;

        // jesli mozna i potrzeba, listujemy dostepne napisy
        if(showList)
        {
            bool ok = false;

            printCli(QString("   0)\tNie pobieraj napisow dla tego filmu"));
            printCli("   Znalezione napisy:");

            int i = 1;

            QList<QNapiSubtitleInfo> list = napi.listSubtitles();


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

            while(!ok)
            {
                std::cout << "   Wybierz napisy do pobrania: ";
                char line[8];
                std::cin.getline(line, 8);

                selIdx = QString(line).toInt(&ok);

                if(!ok)
                {
                    printCli("   Wpisz liczbe!");
                    std::cin.clear();
                }
                else if((selIdx > list.size()) || (selIdx < 0))
                {
                    ok = false;
                    printCli("   Wpisz liczbe z listy!");
                }
            }

            --selIdx;
        }
        else
        {
            selIdx = napi.bestIdx();
        }

        if(selIdx == -1) continue;

        printCli(QString(QString("   Pobieranie napisow z serwera...")));
        if(!napi.download(selIdx))
        {
            printCli(QString(QString("   Nie udalo sie pobrac napisow!")));
            return EC_COULD_NOT_DOWNLOAD;
        }

        printCli(QString(QString("   Rozpakowywanie napisow...")));
        if(!napi.unpack())
        {
            printCli(QString(QString("   Nie udało sie poprawnie rozpakowac napisow!")));
            return EC_COULD_NOT_UNARCHIVE;
        }

        printCli(QString(QString("   Dopasowywanie napisow...")));
        if(!napi.match())
        {
            printCli(QString(QString("   Nie udalo sie dopasowac napisow!")));
            return EC_COULD_NOT_MATCH;
        }

        if(napi.ppEnabled())
        {
            printCli(QString(QString("   Przetwarzanie pobranych napisow...")));
            napi.pp();
        }

        napi.cleanup();
    }

    return EC_OK;
}

void QNapiCli::printHeader()
{
    printCli(QString("QNapi %1, %2\n")
                .arg(QNAPI_DISPLAYABLE_VERSION)
                .arg(QNAPI_URL));
}

void QNapiCli::printHelp()
{
    printCli(QString("QNapi rozprowadzany jest na warunkach licencji GNU General Public License v2.\n"));
    printCli(QString("Skladnia: %1 [opcje] [lista plikow]").arg(QFileInfo(arguments().at(0)).fileName()));
    printCli(QString("Dostepne opcje:"));
    printCli(QString("    -c,  --console          pobieranie napisow z konsoli"));
    printCli(QString("    -q,  --quiet            pobiera napisy nie wypisujac zadnych komunikatow"));
    printCli(QString("                            ani nie pokazujac zadnych okien (implikuje -d)\n"));
    printCli(QString("    -s,  --show-list        pokazuj liste napisow (dziala tylko z -c)"));
    printCli(QString("    -d,  --dont-show-list   nie pokazuj listy napisow (dziala tylko z -c)\n"));
    printCli(QString("    -l,  --lang [jezyk]     preferowany jezyk napisow\n"));
    printCli(QString("    -lb, --lang-backup      zapasowy jezyk napisow\n"));
    printCli(QString("    -o,  --options          wywoluje okno konfiguracji programu (tylko GUI)\n"));
    printCli(QString("    -h,  --help             pokazuje tekst pomocy"));
    printCli(QString("    -hl, --help-languages   listuje jezyki, w jakich mozna pobierac napisy\n"));
}

void QNapiCli::printHelpLanguages()
{
    printCli(QString("Oto lista rozpoznawanych przez QNapi jezykow i odpowiadajacym"));
    printCli(QString("im dwuliterowych kodow:\n"));

    QNapiLanguage L;
    QStringList langs = L.listLanguages();

    foreach(QString lang, langs)
    {
        L.setLanguage(lang);
        printCli(QString(" %1 - %2").arg(L.toTwoLetter()).arg(lang));
    }

    L.setLanguage(GlobalConfig().language());

    printCli(QString("\nAktualnie ustawiony, domyslny jezyk napisow: %1 (%2)")
                .arg(L.toFullName()).arg(L.toTwoLetter()));

}

void QNapiCli::printCli(const QString & string)
{
    if(mode != CM_QUIET)
        std::cout << string.toStdString() << std::endl;
}

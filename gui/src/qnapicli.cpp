/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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
#include "libqnapi.h"

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
                printCli(QString("Invalid language code: %1").arg(p));
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

    bool quietBatch = GlobalConfig().quietBatch();
    if(quietBatch && !movieList.isEmpty())
    {
        mode = CM_QUIET;
        return true;
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
        printCli("File not found!");
        return EC_MOVIE_FILE_NOT_FOUND;
    }

    if(mode == CM_HELP)
    {
        printHelp();
        return EC_OK;
    }

    if(!QNapi::checkP7ZipPath())
    {
        printCli("Path to the program p7zip is incorrect! Check your settings..");
        return EC_P7ZIP_UNAVAILABLE;
    }

    if(!QNapi::checkTmpPath())
    {
        printCli("Can not write to the temporary directory! Check your settings.");
        return EC_CANNOT_WRITE_TMP_DIR;
    }

    
    if(!napi.addEngines(GlobalConfig().enginesList()))
    {
        printCli(QString("Error: ") + napi.error());
        return EC_UNSUPPORTED_ENGINE;
    }

    if(lang.isEmpty())
        lang = GlobalConfig().language();

    if(!langBackupPassed)
        langBackup = GlobalConfig().languageBackup();

    foreach(QString movie, movieList)
    {
        printCli(QString(QString(" * Downloading subtitles for '%1'")).arg(QFileInfo(movie).fileName()));

        napi.setMoviePath(movie);

        if(!napi.checkWritePermissions())
        {
            printCli(QString("   No permission to write to the directory '%1'!").arg(QFileInfo(movie).path()));
            return EC_NO_WRITE_PERMISSIONS;
        }

        napi.clearSubtitlesList();

        printCli(QString(QString("   Calculating checksums...")));
        napi.checksum();

        bool found = false;
        SearchPolicy sp = GlobalConfig().searchPolicy();

        if(sp == SP_SEARCH_ALL_WITH_BACKUP_LANG)
        {
            foreach(QString e, napi.listLoadedEngines())
            {
                printCli(QString(QString("   Searching subtitles [%1] (%2)...").arg(lang, e)));
                found = napi.lookForSubtitles(lang, e) || found;
                printCli(QString(QString("   Searching alternative subtitles [%1] (%2)...").arg(langBackup, e)));
                found = napi.lookForSubtitles(langBackup, e) || found;
            }
        }
        else
        {
            foreach(QString e, napi.listLoadedEngines())
            {
                printCli(QString(QString("   Searching subtitles [%1] (%2)...").arg(lang, e)));
                found = napi.lookForSubtitles(lang, e) || found;

                if(sp == SP_BREAK_IF_FOUND && found)
                    break;
            }

            if(!found && !langBackup.isEmpty()) {
                foreach(QString e, napi.listLoadedEngines())
                {
                    printCli(QString(QString("   Searching alternative subtitles [%1] (%2)...").arg(langBackup, e)));
                    found = napi.lookForSubtitles(langBackup, e) || found;

                    if(sp == SP_BREAK_IF_FOUND && found)
                        break;
                }
            }
        }


        if(!found)
        {
            printCli(QString(QString("   Subtitles not found!")));
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

            printCli(QString("   0)\tDo not download subtitles for this video"));
            printCli("   Found subtitles:");

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
                std::cout << "   Select subtitles to download: ";
                char line[8];
                std::cin.getline(line, 8);

                selIdx = QString(line).toInt(&ok);

                if(!ok)
                {
                    printCli("   You must enter a number!");
                    std::cin.clear();
                }
                else if((selIdx > list.size()) || (selIdx < 0))
                {
                    ok = false;
                    printCli("   Enter a number from the list!");
                }
            }

            --selIdx;
        }
        else
        {
            selIdx = napi.bestIdx();
        }

        if(selIdx == -1) continue;

        printCli(QString(QString("   Downloading subtitles...")));
        if(!napi.download(selIdx))
        {
            printCli(QString(QString("   Unable to download subtitles!")));
            return EC_COULD_NOT_DOWNLOAD;
        }

        printCli(QString(QString("   Unpacking subtitles...")));
        if(!napi.unpack(selIdx))
        {
            printCli(QString(QString("   Failed to unpack subtitles!")));
            return EC_COULD_NOT_UNARCHIVE;
        }

        if(napi.ppEnabled())
        {
            printCli(QString(QString("   Post-processing subtitles file...")));
            napi.pp();
        }

        printCli(QString(QString("   Adjusting subtitles...")));
        if(!napi.match())
        {
            printCli(QString(QString("   Could not adjust subtitles!")));
            return EC_COULD_NOT_MATCH;
        }

        napi.cleanup();
    }

    return EC_OK;
}

void QNapiCli::printHeader()
{
    printCli(QString("QNapi %1, %2\nQt version: %3\n")
                .arg(LibQNapi::displayableVersion())
                .arg(LibQNapi::webpageUrl())
                .arg(qVersion()));
}

void QNapiCli::printHelp()
{
    printCli(QString("QNapi is distributed under the GNU General Public License v2.\n"));
    printCli(QString("Syntax: %1 [options] [list of files]").arg(QFileInfo(arguments().at(0)).fileName()));
    printCli(QString("Available options:"));
    printCli(QString("   -c, --console              Download subtitles with console"));
    printCli(QString("   -q, --quiet                Download subtitles quietly without showing"));
    printCli(QString("                              any messages or windows (implies -d)\n"));
    printCli(QString("   -s, --show-list            Show a list of subtitles (works only with -c)"));
    printCli(QString("   -d, --dont-show-list       Do not show a list of subtitles (works only with -c)\n"));
    printCli(QString("   -l, --lang                 Preferred subtitles language"));
    printCli(QString("   -lb,--lang-backup          Alternative subtitles language\n"));
    printCli(QString("   -o, --options              Show program options (only GUI)\n"));
    printCli(QString("   -h, --help                 Show help text"));
    printCli(QString("   -hl,--help-languages       List of available subtitles languages\n"));
}

void QNapiCli::printHelpLanguages()
{
    printCli(QString("List of languages recognized by QNapi, including corresponding"));
    printCli(QString("two-letter language codes:\n"));

    QNapiLanguage L, LB;
    QStringList langs = L.listLanguages();

    foreach(QString lang, langs)
    {
        L.setLanguage(lang);
        printCli(QString(" %1 - %2").arg(L.toTwoLetter()).arg(lang));
    }

    L.setLanguage(GlobalConfig().language());
    LB.setLanguage(GlobalConfig().languageBackup());

    printCli(QString("\nCurrent default subtitles language: %1 (%2)")
                .arg(L.toFullName()).arg(L.toTwoLetter()));

    if(LB.toFullName().isEmpty()) {
        printCli(QString("No alternative subtitles language has been set"));
    } else {
        printCli(QString("Current alternative subtitles language: %1 (%2)")
                    .arg(LB.toFullName()).arg(LB.toTwoLetter()));
    }
}

void QNapiCli::printCli(const QString & string)
{
    if(mode != CM_QUIET)
        std::cout << string.toStdString() << std::endl;
}

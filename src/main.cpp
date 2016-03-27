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
#include <signal.h>

QStringList parseArgs(int argc, char **argv);
void regSignal();
void sigHandler(int);

int main(int argc, char **argv)
{
    bool isCliCall = QNapiCli::isCliCall(argc, argv);

    QStringList pathList = parseArgs(argc, argv);

    regSignal();

    QFileInfo appExe(argv[0]);
    GlobalConfig().load(appExe.absoluteDir().path());

    bool quietBatch = GlobalConfig().quietBatch();
    bool useGui = !isCliCall && !(quietBatch && !pathList.isEmpty());

    if(useGui)
    {
        QNapiApp app(argc, argv, true, "QNapi");

        QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QTranslator cuteTranslator;
        cuteTranslator.load("qt_" + QLocale::system().name(), resourceDir);
        app.installTranslator(&cuteTranslator);
        app.setQuitOnLastWindowClosed(false);

        if(app.arguments().contains("-o") || app.arguments().contains("--options"))
        {
            app.setQuitOnLastWindowClosed(true);
            app.showSettings();
            return 0;
        }

        if(!app.isInstanceAllowed())
        {
            for(int i = 0; i < pathList.size(); i++) {
                QString & fileName = pathList[i];
                QFileInfo fi(fileName);
                app.sendRequest(fi.absoluteFilePath());
            }
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
        if(pathList.size() > 0)
        {
            app.progress()->setBatchMode(true);

            QString batchLang, batchLangBackup, p;
            bool invalidLang = false, batchLangBackupPassed = false;

            for(int i = 1; i < argc; i++)
            {
                p = argv[i];

                if((p == "-l") || (p == "--lang"))
                {
                    ++i;
                    if(i < argc)
                    {
                        batchLang = QNapiLanguage(argv[i]).toTwoLetter();
                        if(batchLang.isEmpty())
                            invalidLang = true;
                    } else invalidLang = true;

                }
                else if((p == "-lb") || (p == "--lang-backup"))
                {
                    ++i;
                    if(i < argc)
                    {
                        batchLangBackup = QNapiLanguage(argv[i]).toTwoLetter();
                        batchLangBackupPassed = true;
                    }
                    break;
                }
            }

            if(invalidLang)
            {
                if(QMessageBox::question(0, "QNapi", "Niepoprawny kod językowy!\n"
                        "Czy chcesz pobrać napisy w domyślnym języku?",
                        QMessageBox::Yes | QMessageBox::No)
                    != QMessageBox::Yes)
                {
                    return 0;
                }
            }

            app.progress()->setBatchLanguages(batchLang, batchLangBackup, batchLangBackupPassed);

            if(QFileInfo(pathList.at(0)).isDir())
            {
                if(!app.showScanDialog(pathList.at(0)))
                    return 1;
            }
            else
            {
                app.progress()->enqueueFiles(pathList);
                if(!app.progress()->download()) return 1;
            }
        }

        // Jesli nie dzialamy w trybie pobierania, mozemy ew. utworzyc ikone w tray-u
        // badz pokazac okno wyboru plikow z filmami
        if(!app.progress()->isBatchMode())
        {
            app.createMainWindow();

            if(QSystemTrayIcon::isSystemTrayAvailable())
            {
                app.createTrayIcon();
            }
        }

        return app.exec();
    }
    else
    {
        QNapiCli app(argc, argv);
        return app.exec();
    }
}

QStringList parseArgs(int argc, char **argv)
{
    QStringList pathList;

    for(int i = 1; i < argc; i++)
    {
#ifdef Q_OS_WIN
        QString p = QString::fromLocal8Bit(argv[i]);
#else
        QString p = argv[i];
#endif
        if(p.startsWith("file://"))
            p = p.remove(0, 7);

        if((pathList.size() == 0) && QFileInfo(p).isDir())
        {
            pathList << p;
            break;
        }

        if(QFileInfo(p).isFile())
            pathList << p;
    }
    return pathList;
}

void regSignal()
{
#ifdef Q_OS_WIN
    signal(SIGTERM, sigHandler);
    signal(SIGINT, sigHandler);
#else
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigHandler;
    sigaction(SIGTERM, &sa, 0);
    sigaction(SIGINT, &sa, 0);
#endif
}

void sigHandler(int sig)
{
    Q_UNUSED(sig);

    qDebug() << "\nQNapi: usuwanie plików tymczasowych...";

    QString tmpPath = GlobalConfig().tmpPath();

    QStringList filters;
    filters << "QNapi-*-rc";
    filters << "QNapi.*.tmp";

    QDir dir(tmpPath);

    QStringList files = dir.entryList(filters);

    foreach(QString file, files)
    {
        QFile::remove(tmpPath + QDir::separator() + file);
    }

    exit(666);
}

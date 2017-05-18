/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include <signal.h>
#include <QApplication>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QTextCodec>
#include <QTranslator>
#include "libqnapi.h"
#include "qnapiapp.h"
#include "qnapicli.h"

QStringList parseArgs(int argc, char **argv);
void regSignal();
void sigHandler(int);

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);

  const QNapiConfig config = LibQNapi::loadConfig();

  QString uiLanguage = LibQNapi::uiLanguage(config.generalConfig());

  bool isCliCall = QNapiCli::isCliCall(argc, argv);

  QStringList parsedFilePaths = parseArgs(argc, argv);

  regSignal();

  bool quietBatch = config.generalConfig().quietBatch();
#ifdef Q_OS_MAC
  quietBatch = false;
#endif

  bool useGui = !isCliCall && !(quietBatch && !parsedFilePaths.isEmpty());

  if (useGui) {
    QNapiApp app(argc, argv, true, "QNapi");

    QString resourceDir =
        QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    QTranslator qtTranslator, qnapiTranslator;
    qtTranslator.load("qt_" + uiLanguage, resourceDir);
    app.installTranslator(&qtTranslator);

    qnapiTranslator.load("qnapi_" + uiLanguage, ":/translations");
    app.installTranslator(&qnapiTranslator);

    app.setQuitOnLastWindowClosed(false);

    if (app.arguments().contains("-o") ||
        app.arguments().contains("--options")) {
      app.setQuitOnLastWindowClosed(true);
      app.showSettings();
      return 0;
    }

    if (!app.isInstanceAllowed()) {
      for (int i = 0; i < parsedFilePaths.size(); i++) {
        QString &fileName = parsedFilePaths[i];
        QFileInfo fi(fileName);
        app.sendRequest(fi.absoluteFilePath());
      }
      return 0;
    }

    if (config.firstrun()) {
      if (QMessageBox::question(0, QObject::tr("First Run"),
                                QObject::tr("This is the first launch of the "
                                            "program QNapi. Do you want to "
                                            "configure it now?"),
                                QMessageBox::Yes | QMessageBox::No) ==
          QMessageBox::Yes) {
        app.showSettings();
      }
    }

    // Jesli podano parametry, ustawiamy tzw. batch mode
    if (parsedFilePaths.size() > 0) {
      app.progress()->setBatchMode(true);

      QString p, batchLang, batchLangBackup;
      bool invalidLang = false, batchLangBackupPassed = false;

      for (int i = 1; i < argc; i++) {
        p = argv[i];

        if ((p == "-l") || (p == "--lang")) {
          ++i;
          if (i < argc) {
            batchLang = SubtitleLanguage(argv[i]).toTwoLetter();
            if (batchLang.isEmpty()) {
              invalidLang = true;
            }
          } else {
            invalidLang = true;
          }
        } else if ((p == "-lb") || (p == "--lang-backup")) {
          ++i;
          if (i < argc) {
            batchLangBackup = SubtitleLanguage(argv[i]).toTwoLetter();
            batchLangBackupPassed = true;
          }
          break;
        } else if ((p == "--format") || (p == "-f")) {
          ++i;
          if (i < argc) {
            if (!LibQNapi::subtitleFormatsRegistry()
                     ->select(argv[i])
                     .isNull()) {
              app.progress()->setTargetFormatOverride(argv[i]);
            }
          }
        } else if ((p == "--extension") || (p == "-e")) {
          ++i;
          if (i < argc) {
            app.progress()->setTargetExtOverride(argv[i]);
          }
        }
      }

      if (invalidLang) {
        if (QMessageBox::question(0, "QNapi",
                                  "Invalid language code!\n"
                                  "Do you want to download the subtitles in "
                                  "the default language?",
                                  QMessageBox::Yes | QMessageBox::No) !=
            QMessageBox::Yes) {
          return 0;
        }
      }

      app.progress()->setBatchLanguages(batchLang, batchLangBackup,
                                        batchLangBackupPassed);

      if (QFileInfo(parsedFilePaths.at(0)).isDir()) {
        if (!app.showScanDialog(parsedFilePaths.at(0))) return 1;
      } else {
        app.progress()->enqueueFiles(parsedFilePaths);
        if (!app.progress()->download()) return 1;
      }
    }

    // Jesli nie dzialamy w trybie pobierania, mozemy ew. utworzyc ikone w
    // tray-u badz pokazac okno wyboru plikow z filmami
    if (!app.progress()->isBatchMode()) {
      // Jesli nie ma traya, od razu wyswietlamy okienko z wyborem pliku
      if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        if (!app.progress()->isBatchMode()) {
          app.progress()->setBatchMode(true);
          if (!app.showOpenDialog()) return 1;
        }
      } else  // Jesli ikona w tray-u jest obsligiwana, tworzymy ja
      {
        app.createTrayIcon();
      }
    }

    return app.exec();
  } else {
    QNapiCli app(argc, argv, config, uiLanguage);
    return app.exec();
  }
}

QStringList parseArgs(int argc, char **argv) {
  QStringList parsedFilePaths;

  for (int i = 1; i < argc; i++) {
#ifdef Q_OS_WIN
    QString p = QString::fromLocal8Bit(argv[i]);
#else
    QString p = argv[i];
#endif
    if (p.startsWith("file://")) p = p.remove(0, 7);

    if ((parsedFilePaths.size() == 0) && QFileInfo(p).isDir()) {
      parsedFilePaths << p;
      break;
    }

    if (QFileInfo(p).isFile()) parsedFilePaths << p;
  }
  return parsedFilePaths;
}

void regSignal() {
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

void sigHandler(int sig) {
  Q_UNUSED(sig);

  std::cout << std::endl
            << QObject::tr("QNapi: deleting temporary files...").toStdString()
            << std::endl;

  const QNapiConfig config = LibQNapi::loadConfig();
  QString tmpPath = config.generalConfig().tmpPath();
  QDir tmpDir(tmpPath);

  QStringList filters;
  filters << "QNapi-*-rc";
  filters << "QNapi.*.tmp";

  QFileInfoList files = tmpDir.entryInfoList(filters);

  foreach (QFileInfo file, files) { QFile::remove(file.filePath()); }

  std::cout << QObject::tr("QNapi: finished.").toStdString() << std::endl;

  exit(666);
}

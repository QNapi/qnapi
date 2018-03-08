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

#include "climain.h"
#include "clisubtitlesdownloader.h"
#include "libqnapi.h"
#include "qnapicommand.h"
#include "subtitlelanguage.h"
#include "utils/helphelper.h"

#include <signal.h>
#include <QDir>
#include <QTranslator>
#include <iostream>

namespace CliMain {

void installTranslation(QCoreApplication &app, QTranslator *translator,
                        const QNapiConfig &config) {
  QString uiLanguage = LibQNapi::uiLanguage(config.generalConfig());
  translator->load("qnapi_" + uiLanguage, ":/translations");
  app.installTranslator(translator);
}

void printHeader(const Console &c) {
  c.printLine(tr("QNapi %1 (Qt version %2), %3")
                  .arg(LibQNapi::displayableVersion())
                  .arg(qVersion())
                  .arg(LibQNapi::webpageUrl()));
  c.printLine();
}

void printHelp(const Console &c,
               const QList<QSharedPointer<CliArgParser>> &cliArgParsers) {
  auto helpLines = HelpHelper::formatHelpLinesText(cliArgParsers);
  foreach (auto helpLine, helpLines) { c.printLine(helpLine); }
}

void printHelpLanguages(const Console &c, const QNapiConfig &config) {
  c.printLine(
      tr("List of languages recognized by QNapi, including corresponding"));
  c.printLine(tr("two-letter language codes:"));
  c.printLine();

  SubtitleLanguage L, LB;
  QStringList langs = L.listLanguages();

  foreach (QString lang, langs) {
    L.setLanguage(lang);
    c.printLine(QString(" %1 - %2").arg(L.toTwoLetter()).arg(lang));
  }

  L.setLanguage(config.generalConfig().language());
  LB.setLanguage(config.generalConfig().backupLanguage());

  c.printLine();
  c.printLine(tr("Current default subtitles language: %1 (%2)")
                  .arg(L.toFullName())
                  .arg(L.toTwoLetter()));

  if (LB.toFullName().isEmpty()) {
    c.printLine(tr("No alternative subtitles language has been set"));
  } else {
    c.printLine(tr("Current alternative subtitles language: %1 (%2)")
                    .arg(LB.toFullName())
                    .arg(LB.toTwoLetter()));
  }
}

int processCommand(QVariant cliCommand, const QNapiConfig &config,
                   const QList<QSharedPointer<CliArgParser>> &cliArgParsers) {
  const Console c(config.generalConfig().quietBatch());

  printHeader(c);

  using namespace QNapiCommand;
  if (cliCommand.canConvert<DownloadSubtitles>()) {
    QStringList movieFilePaths =
        cliCommand.value<DownloadSubtitles>().movieFilePaths;
    return CliSubtitlesDownloader::downloadSubtitlesFor(c, movieFilePaths,
                                                        config);
  } else if (cliCommand.canConvert<ShowHelpLanguages>()) {
    printHelpLanguages(c, config);
    return 0;
  } else {
    printHelp(c, cliArgParsers);
    return 0;
  }
}

void sigHandler(int sig) {
  Q_UNUSED(sig);

  std::cout << std::endl
            << tr("QNapi: deleting temporary files...").toStdString()
            << std::endl;

  const QNapiConfig config = LibQNapi::loadConfig();
  QString tmpPath = config.generalConfig().tmpPath();
  QDir tmpDir(tmpPath);

  QStringList filters;
  filters << "QNapi-*-rc";
  filters << "QNapi.*.tmp";

  QFileInfoList files = tmpDir.entryInfoList(filters);

  foreach (QFileInfo file, files) { QFile::remove(file.filePath()); }

  std::cout << tr("QNapi: finished.").toStdString() << std::endl;

  exit(666);
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

}  // namespace CliMain

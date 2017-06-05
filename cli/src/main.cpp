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

#include "clisubtitlesdownloader.h"
#include "commandargsparser.h"
#include "console.h"
#include "libqnapi.h"
#include "namespace_tr.h"
#include "qnapiclicommand.h"
#include "subtitlelanguage.h"

#include <QCoreApplication>
#include <QTranslator>
#include <QVariant>

#include <iostream>

namespace Main {

Q_DECLARE_NAMESPACE_TR(Main)

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

void printHelp(const Console &c) {
  QString formats =
      LibQNapi::subtitleFormatsRegistry()->listFormatNames().join(",");
  QString binaryFileName =
      QFileInfo(LibQNapi::appExecutableFilePath).fileName();

  c.printLine(
      tr("QNapi is distributed under the GNU General Public License v2."));
  c.printLine();
  c.printLine(tr("Syntax: %1 [options] [list of files]").arg(binaryFileName));
  c.printLine(tr("Available options:"));
  c.printLine(
      tr("   -q, --quiet                Download subtitles quietly without "
         "showing"));
  c.printLine(
      tr("                              any messages or windows (implies -d)"));
  c.printLine();
  c.printLine(
      tr("   -s, --show-list            Show a list of subtitles (works only "
         "with -c)"));
  c.printLine(
      tr("   -d, --dont-show-list       Do not show a list of subtitles (works "
         "only with -c)"));
  c.printLine();
  c.printLine(tr("   -l, --lang [code]          Preferred subtitles language"));
  c.printLine(
      tr("   -lb,--lang-backup [code]   Alternative subtitles language"));
  c.printLine(tr("   -f, --format [format]      Select target subtitles file "
                 "format (%1)")
                  .arg(formats));
  c.printLine(tr(
      "   -e, --extension [ext]      Select target subtitles file extension"));
  c.printLine();
  c.printLine(tr("   -h, --help                 Show help text"));
  c.printLine(tr(
      "   -hl,--help-languages       List of available subtitles languages"));
  c.printLine();
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

int processCommand(QVariant cliCommand, const QNapiConfig &config) {
  const Console c(config.generalConfig().quietBatch());

  printHeader(c);

  using namespace QNapiCliCommand;
  if (cliCommand.canConvert<DownloadSubtitles>()) {
    QStringList movieFilePaths =
        cliCommand.value<DownloadSubtitles>().movieFilePaths;
    return CliSubtitlesDownloader::downloadSubtitlesFor(c, movieFilePaths,
                                                        config);
  } else if (cliCommand.canConvert<ShowHelpLanguages>()) {
    printHelpLanguages(Console(), config);
    return 0;
  } else {
    printHelp(Console());
    return 0;
  }
}

}  // namespace Main

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);

  const QNapiConfig config = LibQNapi::loadConfig();

  QCoreApplication cliApp(argc, argv);

  QTranslator qnapiTranslator;
  Main::installTranslation(cliApp, &qnapiTranslator, config);

  QStringList tailArgs =
      cliApp.arguments().mid(1, cliApp.arguments().size() - 1);

  auto parseResult = CommandArgsParser::parse(tailArgs, config);

  if (parseResult.is<CommandArgsParser::ParsedCommand>()) {
    auto parsedCommand = parseResult.as<CommandArgsParser::ParsedCommand>();
    return Main::processCommand(parsedCommand.command,
                                parsedCommand.refinedConfig);

  } else {
    const Console c;
    Main::printHeader(c);
    c.printLine(Main::tr("Command line argument parsing error:"));
    c.printLine(parseResult.as<QString>());
    return -1;
  }
}

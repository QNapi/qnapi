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
#include "libqnapi.h"
#include "qnapiclicommand.h"

#include <QCoreApplication>
#include <QTranslator>
#include <QVariant>

#include <iostream>

void processCommand(QVariant cliCommand, const QNapiConfig &config);
void printHelp();
void printHelpLanguages();

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);
  const QNapiConfig config = LibQNapi::loadConfig();
  QString uiLanguage = LibQNapi::uiLanguage(config.generalConfig());
  QCoreApplication cliApp(argc, argv);
  QTranslator qnapiTranslator;
  qnapiTranslator.load("qnapi_" + uiLanguage, ":/translations");
  cliApp.installTranslator(&qnapiTranslator);
  auto parseResult = CommandArgsParser::parse(cliApp.arguments(), config);
  processCommand(parseResult.command, parseResult.refinedConfig);
  return 0;
}

void processCommand(QVariant cliCommand, const QNapiConfig &config) {
  using namespace QNapiCliCommand;
  if (cliCommand.canConvert<DownloadSubtitles>()) {
    QStringList movieFilePaths =
        cliCommand.value<DownloadSubtitles>().movieFilePaths;
    CliSubtitlesDownloader::downloadSubtitlesFor(movieFilePaths, config);
  } else if (cliCommand.canConvert<ShowHelpLanguages>()) {
    printHelpLanguages();
  } else {
    printHelp();
  }
}

void printHelp() { std::cout << "help text" << std::endl; }

void printHelpLanguages() { std::cout << "help languages text" << std::endl; }

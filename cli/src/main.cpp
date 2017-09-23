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

#include "Either.h"
#include "Maybe.h"
#include "climain.h"
#include "libqnapi.h"
#include "parser/backuplangargparser.h"
#include "parser/cliargparser.h"
#include "parser/cliargparsersexecutor.h"
#include "parser/dontshowlistargparser.h"
#include "parser/downloadsubtitlesargparser.h"
#include "parser/extensionargparser.h"
#include "parser/formatargparser.h"
#include "parser/langargparser.h"
#include "parser/quietbatchargparser.h"
#include "parser/showhelpargparser.h"
#include "parser/showhelplanguagesargparser.h"
#include "parser/showlistargparser.h"
#include "qnapicommand.h"
#include "utils/console.h"

#include <QCoreApplication>
#include <QSharedPointer>
#include <QTranslator>

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);

  const QNapiConfig config = LibQNapi::loadConfig();

  QCoreApplication cliApp(argc, argv);

  CliMain::regSignal();

  QTranslator qnapiTranslator;
  CliMain::installTranslation(cliApp, &qnapiTranslator, config);

  const QList<QSharedPointer<CliArgParser>> cliArgParsers = {
      QSharedPointer<CliArgParser>(new ShowHelpArgParser()),
      QSharedPointer<CliArgParser>(new ShowHelpLanguagesArgParser()),
      QSharedPointer<CliArgParser>(new QuietBatchArgParser()),
      QSharedPointer<CliArgParser>(new ShowListArgParser()),
      QSharedPointer<CliArgParser>(new DontShowListArgParser()),
      QSharedPointer<CliArgParser>(new LangArgParser()),
      QSharedPointer<CliArgParser>(new BackupLangArgParser()),
      QSharedPointer<CliArgParser>(new FormatArgParser()),
      QSharedPointer<CliArgParser>(new ExtensionArgParser()),
      QSharedPointer<CliArgParser>(new DownloadSubtitlesArgParser())};

  QStringList tailArgs =
      cliApp.arguments().mid(1, cliApp.arguments().size() - 1);

  auto parseResult =
      CliArgParsersExecutor::executeParsers(cliArgParsers, tailArgs, config);

  const Console c;

  if (parseResult.is<Maybe<CliArgParser::ParsedCommand>>()) {
    auto parsedMaybeCommand =
        parseResult.as<Maybe<CliArgParser::ParsedCommand>>();

    if (parsedMaybeCommand) {
      auto parsedCommand = parsedMaybeCommand.value();
      return CliMain::processCommand(
          parsedCommand.command, parsedCommand.refinedConfig, cliArgParsers);
    } else {
      CliMain::printHeader(c);
      CliMain::printHelp(c, cliArgParsers);
      return 0;
    }
  } else {
    CliMain::printHeader(c);
    c.printLine(CliMain::tr("Command line argument parsing error:"));
    c.printLine(parseResult.as<QString>());
    return -1;
  }
}

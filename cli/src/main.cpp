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
#include "commandargsparser.h"
#include "console.h"
#include "libqnapi.h"
#include "qnapiclicommand.h"

#include <QCoreApplication>
#include <QTranslator>

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);

  const QNapiConfig config = LibQNapi::loadConfig();

  QCoreApplication cliApp(argc, argv);

  CliMain::regSignal();

  QTranslator qnapiTranslator;
  CliMain::installTranslation(cliApp, &qnapiTranslator, config);

  QStringList tailArgs =
      cliApp.arguments().mid(1, cliApp.arguments().size() - 1);

  auto parseResult = CommandArgsParser::parse(tailArgs, config);

  if (parseResult.is<CommandArgsParser::ParsedCommand>()) {
    auto parsedCommand = parseResult.as<CommandArgsParser::ParsedCommand>();
    return CliMain::processCommand(parsedCommand.command,
                                   parsedCommand.refinedConfig);
  } else {
    const Console c;
    CliMain::printHeader(c);
    c.printLine(CliMain::tr("Command line argument parsing error:"));
    c.printLine(parseResult.as<QString>());
    return -1;
  }
}

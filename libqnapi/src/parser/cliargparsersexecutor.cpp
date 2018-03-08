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

#include "cliargparsersexecutor.h"

namespace CliArgParsersExecutor {

Either<QString, Maybe<CliArgParser::ParsedCommand>> executeParsers(
    const QList<QSharedPointer<CliArgParser>> &parsers, const QStringList &args,
    const QNapiConfig &config) {
  QNapiConfig refinedConfig = config;

  foreach (auto cliArgParser, parsers) {
    auto result = cliArgParser->parse(args, refinedConfig);

    if (result.canConvert<CliArgParser::ParseError>()) {
      return some(result.value<CliArgParser::ParseError>().errorMessage);
    } else if (result.canConvert<CliArgParser::ParsedCommand>()) {
      Maybe<CliArgParser::ParsedCommand> maybeParsedCommand =
          just(result.value<CliArgParser::ParsedCommand>());
      return some(maybeParsedCommand);
    } else if (result.canConvert<CliArgParser::ParsedModifier>()) {
      refinedConfig =
          result.value<CliArgParser::ParsedModifier>().refinedConfig;
    }
  }

  Maybe<CliArgParser::ParsedCommand> n = nothing();
  return some(n);
}

QStringList formatHelpLines(const QList<QSharedPointer<CliArgParser>> &parsers,
                            int switchesBlockSize, int descBlockSize,
                            int preSwitchSpaceSize) {
  QString preSwitchSpace(preSwitchSpaceSize, ' ');
  QStringList helpLines;

  foreach (auto parser, parsers) {
    auto maybeHelpInfo = parser->helpInfo();

    if (maybeHelpInfo) {
      auto helpInfo = maybeHelpInfo.value();

      QString optSwitches = preSwitchSpace + helpInfo.shortSwitch + ",";
      if (helpInfo.shortSwitch.size() == 2) optSwitches += " ";
      optSwitches += helpInfo.longSwitch;
      if (!helpInfo.argName.isEmpty()) {
        optSwitches += QString(" [%1]").arg(helpInfo.argName);
      }
      if (optSwitches.length() < switchesBlockSize) {
        optSwitches += QString(switchesBlockSize - optSwitches.length(), ' ');
      }

      QString notConsumedDescription = helpInfo.description.trimmed();
      while (!notConsumedDescription.isEmpty()) {
        QString helpPart;

        if (notConsumedDescription.size() <= descBlockSize) {
          helpPart = notConsumedDescription;
          notConsumedDescription = "";
        } else {
          int lastSpace =
              notConsumedDescription.left(1 + descBlockSize).lastIndexOf(' ');
          helpPart = notConsumedDescription.left(lastSpace);
          notConsumedDescription =
              notConsumedDescription.mid(lastSpace).trimmed();
        }

        helpLines << optSwitches + helpPart;
        optSwitches = QString(switchesBlockSize, ' ');
      }
    }
  }

  return helpLines;
}

}  // namespace CliArgParsersExecutor

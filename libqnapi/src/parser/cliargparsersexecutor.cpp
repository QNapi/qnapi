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
    } else {
      return some(tr("General parsing error."));
    }
  }

  Maybe<CliArgParser::ParsedCommand> n = nothing();
  return some(n);
}

}  // namespace CliArgParsersExecutor

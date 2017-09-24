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

#include "runcliappargparser.h"
#include "qnapicommand.h"

RunCLIAppArgParser::RunCLIAppArgParser() {}

QVariant RunCLIAppArgParser::parse(const QStringList& args,
                                   const QNapiConfig& config) const {
  if (args.contains("-c") || args.contains("--console")) {
    QStringList argsRemovedCLISwitch = args;
    argsRemovedCLISwitch.removeAll("-c");
    argsRemovedCLISwitch.removeAll("--console");
    return QVariant::fromValue(ParsedCommand{
        config,
        QVariant::fromValue(QNapiCommand::RunCLIApp{argsRemovedCLISwitch})});
  } else {
    return QVariant::fromValue(NothingParsed());
  }
}

Maybe<CliArgParser::HelpInfo> RunCLIAppArgParser::helpInfo() const {
  return just(
      HelpInfo{"-c", "--console", "",
               tr("Run in command-line mode (requires separate CLI binary)")});
}

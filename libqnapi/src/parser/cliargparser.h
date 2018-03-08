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

#ifndef CLIARGPARSER_H
#define CLIARGPARSER_H

#include "config/qnapiconfig.h"

#include <QMetaType>
#include <QStringList>
#include "Either.h"
#include "Maybe.h"

class CliArgParser {
 public:
  struct ParseError {
    QString errorMessage;
  };

  struct NothingParsed {};

  struct ParsedModifier {
    const QNapiConfig refinedConfig;
  };

  struct ParsedCommand {
    const QNapiConfig refinedConfig;
    QVariant command;
  };

  struct HelpInfo {
    QString shortSwitch;
    QString longSwitch;
    QString argName;
    QString description;
  };

  virtual ~CliArgParser();

  virtual QVariant parse(const QStringList& args,
                         const QNapiConfig& config) const = 0;

  virtual Maybe<HelpInfo> helpInfo() const { return nothing(); }
};

Q_DECLARE_METATYPE(CliArgParser::ParseError)
Q_DECLARE_METATYPE(CliArgParser::NothingParsed)
Q_DECLARE_METATYPE(CliArgParser::ParsedModifier)
Q_DECLARE_METATYPE(CliArgParser::ParsedCommand)
Q_DECLARE_METATYPE(Maybe<CliArgParser::ParsedCommand>)
Q_DECLARE_METATYPE(CliArgParser::HelpInfo)
Q_DECLARE_METATYPE(Maybe<CliArgParser::HelpInfo>)

#endif  // CLIARGPARSER_H

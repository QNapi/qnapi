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

#ifndef COMMANDARGSPARSER_H
#define COMMANDARGSPARSER_H

#include <Either.h>

#include "config/qnapiconfig.h"
#include "namespace_tr.h"
namespace CommandArgsParser {

Q_DECLARE_NAMESPACE_TR(CommandArgsParser)

struct ParsedCommand {
  QNapiConfig refinedConfig;
  QVariant command;
};

Either<QString, ParsedCommand> parse(const QStringList& args,
                                     const QNapiConfig& config);

}  // namespace CommandArgsParser

Q_DECLARE_METATYPE(CommandArgsParser::ParsedCommand)

#endif  // COMMANDARGSPARSER_H

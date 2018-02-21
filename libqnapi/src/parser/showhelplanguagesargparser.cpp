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

#include "showhelplanguagesargparser.h"
#include "qnapicommand.h"

ShowHelpLanguagesArgParser::ShowHelpLanguagesArgParser() {}

QVariant ShowHelpLanguagesArgParser::parse(const QStringList& args,
                                           const QNapiConfig& config) const {
  if (args.contains("-hl") || args.contains("--help-languages")) {
    return QVariant::fromValue(ParsedCommand{
        config, QVariant::fromValue(QNapiCommand::ShowHelpLanguages())});
  } else {
    return QVariant::fromValue(NothingParsed());
  }
}

Maybe<CliArgParser::HelpInfo> ShowHelpLanguagesArgParser::helpInfo() const {
  return just(HelpInfo{"-hl", "--help-languages", "",
                       tr("List of available subtitles languages")});
}

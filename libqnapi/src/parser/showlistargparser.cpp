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

#include "showlistargparser.h"
#include "qnapicommand.h"

ShowListArgParser::ShowListArgParser() {}

QVariant ShowListArgParser::parse(const QStringList& args,
                                  const QNapiConfig& config) const {
  if (args.contains("-s") || args.contains("--show-list")) {
    return QVariant::fromValue(ParsedModifier{
        config.setGeneralConfig(config.generalConfig().setDownloadPolicy(
            DownloadPolicy::DP_ALWAYS_SHOW_LIST))});
  } else {
    return QVariant::fromValue(NothingParsed());
  }
}

Maybe<CliArgParser::HelpInfo> ShowListArgParser::helpInfo() const {
  return just(HelpInfo{"-s", "--show-list", "",
                       tr("Always show a list of available subtitles")});
}

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

#include "quietbatchargparser.h"
#include "qnapicommand.h"

QuietBatchArgParser::QuietBatchArgParser() {}

QVariant QuietBatchArgParser::parse(const QStringList& args,
                                    const QNapiConfig& config) const {
  if (args.contains("-q") || args.contains("--quiet")) {
    return QVariant::fromValue(ParsedModifier{
        config.setGeneralConfig(config.generalConfig().setQuietBatch(true))});
  } else {
    return QVariant::fromValue(NothingParsed());
  }
}

Maybe<CliArgParser::HelpInfo> QuietBatchArgParser::helpInfo() const {
  return just(
      HelpInfo{"-q", "--quiet", "",
               tr("Download subtitles silently without showing any messages")});
}

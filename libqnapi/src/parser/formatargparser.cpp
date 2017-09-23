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

#include "formatargparser.h"
#include "libqnapi.h"

FormatArgParser::FormatArgParser() {}

QVariant FormatArgParser::parse(const QStringList& args,
                                const QNapiConfig& config) const {
  int idx = args.indexOf("-f");
  if (idx == -1) {
    idx = args.indexOf("--format");
  }
  if (idx == -1) {
    return QVariant::fromValue(NothingParsed());
  }

  if (idx == args.size() - 1) {
    return QVariant::fromValue(ParseError{
        tr("You need to pass subtitles format code after %1 argument")
            .arg(args[idx])});
  }

  QString format = args[idx + 1];

  if (LibQNapi::subtitleFormatsRegistry()->select(format).isNull()) {
    return QVariant::fromValue(ParseError{
        tr("Invalid target subtitles format passed: %1").arg(format)});
  } else {
    return QVariant::fromValue(ParsedModifier{config.setPostProcessingConfig(
        config.postProcessingConfig().setSubFormat(format))});
  }
}

Maybe<CliArgParser::HelpInfo> FormatArgParser::helpInfo() const {
  return just(HelpInfo{
      "-f", "--format", tr("format"),
      tr("Select target subtitles file format (one of mDVD, MPL2, SRT, TMP)")});
}

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

#include "commandargsparser.h"
#include "qnapiclicommand.h"

namespace CommandArgsParser {

using namespace QNapiCliCommand;

Result parse(const QStringList& args, const QNapiConfig& config) {
  // TODO: implement real command parsing

  if (args.contains("-h")) {
    return Result{config, QVariant::fromValue(ShowHelp())};
  }

  if (args.contains("-hl")) {
    return Result{config, QVariant::fromValue(ShowHelpLanguages())};
  }

  if (args.size() > 1) {
    return Result{config, QVariant::fromValue(DownloadSubtitles{args})};
  }

  return Result{config, QVariant::fromValue(ShowHelp())};
}

}  // namespace CommandArgsParser

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

#include "helphelper.h"
#include "libqnapi.h"
#include "parser/cliargparsersexecutor.h"

namespace HelpHelper {

QStringList formatHelpLinesText(
    const QList<QSharedPointer<CliArgParser>> &cliArgParsers,
    int switchesBlockSize, int descBlockSize, int preSwitchSpaceSize) {
  QString binaryFileName =
      QFileInfo(LibQNapi::appExecutableFilePath).fileName();

  QStringList helpLines;

  helpLines << tr(
      "QNapi is distributed under the GNU General Public License v2.");
  helpLines << "";

  helpLines << tr("Syntax: %1 [options] [list of files]").arg(binaryFileName);
  helpLines << tr("Available options:");

  helpLines << CliArgParsersExecutor::formatHelpLines(
      cliArgParsers, switchesBlockSize, descBlockSize, preSwitchSpaceSize);

  return helpLines;
}

}  // namespace HelpHelper

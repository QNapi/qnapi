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

#ifndef HELPHELPER_H
#define HELPHELPER_H

#include <QSharedPointer>
#include <QStringList>
#include "parser/cliargparser.h"
#include "tr.h"

namespace HelpHelper {
Q_DECLARE_NAMESPACE_TR(HelpHelper)

QStringList formatHelpLinesText(
    const QList<QSharedPointer<CliArgParser>> &cliArgParsers,
    int switchesBlockSize = 30, int descBlockSize = 50,
    int preSwitchSpaceSize = 2);

};  // namespace HelpHelper

#endif  // HELPHELPER_H

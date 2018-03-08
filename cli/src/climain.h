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

#ifndef CLIMAIN_H
#define CLIMAIN_H

#include "config/qnapiconfig.h"
#include "parser/cliargparser.h"
#include "parser/cliargparsersexecutor.h"
#include "tr.h"
#include "utils/console.h"

namespace CliMain {

Q_DECLARE_NAMESPACE_TR(CliMain)

void installTranslation(QCoreApplication &app, QTranslator *translator,
                        const QNapiConfig &config);
void printHeader(const Console &c);
void printHelp(const Console &c,
               const QList<QSharedPointer<CliArgParser>> &cliArgParsers);
void printHelpLanguages(const Console &c, const QNapiConfig &config);
int processCommand(QVariant cliCommand, const QNapiConfig &config,
                   const QList<QSharedPointer<CliArgParser>> &cliArgParsers);
void sigHandler(int sig);
void regSignal();

}  // namespace CliMain

#endif  // CLIMAIN_H

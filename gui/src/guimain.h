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

#ifndef GUIMAIN_H
#define GUIMAIN_H

#include "config/qnapiconfig.h"
#include "namespace_tr.h"
#include "qnapiapp.h"

#include <QCoreApplication>
#include <QTranslator>

namespace GuiMain {
Q_DECLARE_NAMESPACE_TR(GuiMain)

int processCommand(QNapiApp &app, QVariant cliCommand,
                   const QNapiConfig &config);
void showArgParserError(const QString &errorMessage);

bool isCLIAvailable();
int runCLI(const QStringList &args);
void installTranslation(QCoreApplication &app, QTranslator *qtTranslator,
                        QTranslator *translator, const QNapiConfig &config);
void regSignal();
void sigHandler(int);
}  // namespace GuiMain

#endif  // GUIMAIN_H

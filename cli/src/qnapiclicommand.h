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

#ifndef QNAPICLICOMMAND_H
#define QNAPICLICOMMAND_H

#include <QMetaType>
#include <QStringList>

namespace QNapiCliCommand {

struct DownloadSubtitles {
  QStringList movieFilePaths;
};

struct ShowHelp {};

struct ShowHelpLanguages {};

}  // namespace QNapiCliCommand

Q_DECLARE_METATYPE(QNapiCliCommand::DownloadSubtitles)
Q_DECLARE_METATYPE(QNapiCliCommand::ShowHelp)
Q_DECLARE_METATYPE(QNapiCliCommand::ShowHelpLanguages)

#endif  // QNAPICLICOMMAND_H

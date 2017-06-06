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

#ifndef QNAPIGUICOMMAND_H
#define QNAPIGUICOMMAND_H

#include <QMetaType>
#include <QStringList>
#include <QVariant>

namespace QNapiGuiCommand {

struct DownloadSubtitles {
  QStringList movieFilePaths;
};

struct ScanDirectory {
  QString directory;
};

// not implemented yet
// struct ConvertSubtitles {
//  QString subtitlesPath;
//};

struct ShowOptions {};

struct ShowHelp {};

struct ShowHelpLanguages {};

}  // namespace QNapiGuiCommand

Q_DECLARE_METATYPE(QNapiGuiCommand::DownloadSubtitles)
Q_DECLARE_METATYPE(QNapiGuiCommand::ScanDirectory)
// not implemented yet
// Q_DECLARE_METATYPE(QNapiGuiCommand::ConvertSubtitles)
Q_DECLARE_METATYPE(QNapiGuiCommand::ShowOptions)
Q_DECLARE_METATYPE(QNapiGuiCommand::ShowHelp)
Q_DECLARE_METATYPE(QNapiGuiCommand::ShowHelpLanguages)

#endif  // QNAPIGUICOMMAND_H

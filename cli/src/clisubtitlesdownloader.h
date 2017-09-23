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

#ifndef CLISUBTITLESDOWNLOADER_H
#define CLISUBTITLESDOWNLOADER_H

#include "config/qnapiconfig.h"
#include "tr.h"
#include "utils/console.h"

#include <QStringList>

namespace CliSubtitlesDownloader {

Q_DECLARE_NAMESPACE_TR(CliSubtitlesDownloader)

int downloadSubtitlesFor(const Console& c, const QStringList& movieFilePaths,
                         const QNapiConfig& config);
};  // namespace CliSubtitlesDownloader

#endif  // CLISUBTITLESDOWNLOADER_H

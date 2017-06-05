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

#include "clisubtitlesdownloader.h"

#include <QFileInfo>
#include <iostream>

namespace CliSubtitlesDownloader {

int configChecks(const Console& c, const QNapiConfig& config) {
  QString p7zipPath = config.generalConfig().p7zipPath();
  if (!QFileInfo(p7zipPath).isExecutable()) {
    c.printLine(tr("Invalid path to p7zip executable: %1").arg(p7zipPath));
    return 2;
  }

  QString tmpPath = config.generalConfig().tmpPath();
  QFileInfo tmp(tmpPath);
  if (!tmp.isDir() || !tmp.isWritable()) {
    c.printLine(tr("Invalid path to temporary directory: %1").arg(tmpPath));
    return 3;
  }

  return 0;
}

int downloadSubtitlesFor(const Console& c, const QStringList& movieFilePaths,
                         const QNapiConfig& config) {
  int ccErr = configChecks(c, config);
  if (ccErr != 0) {
    return ccErr;
  }

  std::cout << "downloading subtitles for "
            << movieFilePaths.join(";").toStdString() << std::endl;

  std::cout << "config: " << std::endl << config.toString().toStdString();
}
};  // namespace CliSubtitlesDownloader

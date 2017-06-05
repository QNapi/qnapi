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
#include "qnapi.h"

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

int downloadForMovie(const Console& c, const QString& movieFilePath,
                     const QNapiConfig& config, QNapi& napi) {
  QString movieFileName = QFileInfo(movieFilePath).fileName();
  c.printLineHighlihted(
      tr("Downloading subtitles for '%1'").arg(movieFileName));

  napi.setMoviePath(movieFilePath);

  if (!napi.checkWritePermissions()) {
    c.printLineError(tr("No permission to write to the directory '%1'!")
                         .arg(QFileInfo(movieFilePath).path()));
    return 5;
  }

  napi.clearSubtitlesList();

  c.printLineOrdinary(tr("Calculating checksums..."));
  napi.checksum();

  return 0;
}

int downloadSubtitlesFor(const Console& c, const QStringList& movieFilePaths,
                         const QNapiConfig& config) {
  int ccErr = configChecks(c, config);
  if (ccErr != 0) {
    return ccErr;
  }

  c.printLine("------- CONFIG --------");
  c.printLine(config.toString());
  c.printLine("------- CONFIG --------");

  QNapi napi(config);

  foreach (QString movieFilePath, movieFilePaths) {
    int result = downloadForMovie(c, movieFilePath, config, napi);
    if (result != 0) {
      return result;
    }
  }

  return 0;
}
};  // namespace CliSubtitlesDownloader

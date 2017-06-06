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

enum ExitCode {
  EC_OK = 0,
  EC_P7ZIP_UNAVAILABLE = 2,
  EC_CANNOT_WRITE_TMP_DIR = 3,
  EC_NO_WRITE_PERMISSIONS = 5,
  EC_SUBTITLES_NOT_FOUND = 6,
  EC_COULD_NOT_DOWNLOAD = 7,
  EC_COULD_NOT_UNARCHIVE = 8,
  EC_COULD_NOT_MATCH = 9
};

int configChecks(const Console& c, const QNapiConfig& config) {
  QString p7zipPath = config.generalConfig().p7zipPath();
  if (!QFileInfo(p7zipPath).isExecutable()) {
    c.printLine(tr("Invalid path to p7zip executable: %1").arg(p7zipPath));
    return EC_P7ZIP_UNAVAILABLE;
  }

  QString tmpPath = config.generalConfig().tmpPath();
  QFileInfo tmp(tmpPath);
  if (!tmp.isDir() || !tmp.isWritable()) {
    c.printLine(tr("Can't write to temporary directory: %1").arg(tmpPath));
    return EC_CANNOT_WRITE_TMP_DIR;
  }

  return EC_OK;
}

bool findSubtitles(const Console& c, const QNapiConfig& config, QNapi& napi) {
  bool found = false;
  c.printLineOrdinary(tr("Calculating checksums..."));
  napi.checksum();

  SearchPolicy sp = config.generalConfig().searchPolicy();
  QString lang = config.generalConfig().language();
  QString langBackup = config.generalConfig().backupLanguage();

  if (sp == SP_SEARCH_ALL_WITH_BACKUP_LANG) {
    foreach (QString e, napi.listLoadedEngines()) {
      c.printLineOrdinary(
          tr("Searching for subtitles [%1] (%2)...").arg(lang, e));
      found = napi.lookForSubtitles(lang, e) || found;
      c.printLineOrdinary(
          tr("Searching for subtitles in alternative language [%1] (%2)...")
              .arg(langBackup, e));
      found = napi.lookForSubtitles(langBackup, e) || found;
    }
  } else {
    foreach (QString e, napi.listLoadedEngines()) {
      c.printLineOrdinary(
          tr("Searching for subtitles [%1] (%2)...").arg(lang, e));
      found = napi.lookForSubtitles(lang, e) || found;

      if (sp == SP_BREAK_IF_FOUND && found) break;
    }

    if (!found && !langBackup.isEmpty()) {
      foreach (QString e, napi.listLoadedEngines()) {
        c.printLineOrdinary(
            tr("Searching for subtitles in alternative language [%1] (%2)...")
                .arg(langBackup, e));
        found = napi.lookForSubtitles(langBackup, e) || found;

        if (sp == SP_BREAK_IF_FOUND && found) break;
      }
    }
  }

  return found;
}

void printSubtitlesList(const Console& c, QNapi& napi) {
  c.printLineOrdinary(tr("0)\tDo not download subtitles for this video"));
  c.printLineOrdinary(tr("Found subtitles:"));

  int i = 1;

  foreach (SubtitleInfo s, napi.listSubtitles()) {
    QString resolution = "";

    if (s.resolution == SUBTITLE_GOOD)
      resolution = tr(" (good)");
    else if (s.resolution == SUBTITLE_BAD)
      resolution = tr(" (bad)");

    c.printLineOrdinary(QString("%1)\t%2 (%3) (%4) (%5)%6")
                            .arg(i++)
                            .arg(s.name)
                            .arg(s.format)
                            .arg(s.lang)
                            .arg(s.engine)
                            .arg(resolution));
  }
}

Maybe<int> selectSubtitles(const Console& c, const QNapiConfig& config,
                           QNapi& napi) {
  bool showList = false;
  bool napiShowList = napi.needToShowList();

  if(config.generalConfig().quietBatch() || config.generalConfig().downloadPolicy() == DP_NEVER_SHOW_LIST) {
    showList = false;
  } else if (config.generalConfig().downloadPolicy() == DP_ALWAYS_SHOW_LIST) {
    showList = true;
  } else {
    showList = napiShowList;
  }

  if (!showList) {
    return just(napi.bestIdx());
  } else {
    QList<SubtitleInfo> subtitlesList = napi.listSubtitles();

    printSubtitlesList(c, napi);
    int selIdx = c.inputNumber(tr("Select subtitles to download: "), 0,
                               subtitlesList.size());
    if (selIdx == 0) {
      return nothing();
    } else {
      return just(selIdx - 1);
    }
  }
}

int finishSubtitles(int selIdx, const Console& c, QNapi& napi) {
  c.printLineOrdinary(tr("Downloading subtitles..."));
  if (!napi.download(selIdx)) {
    c.printLineError(tr("Unable to download subtitles!"));
    return EC_COULD_NOT_DOWNLOAD;
  }

  c.printLineOrdinary(tr("Unpacking subtitles..."));
  if (!napi.unpack(selIdx)) {
    c.printLineError(tr("Failed to unpack subtitles!"));
    return EC_COULD_NOT_UNARCHIVE;
  }

  if (napi.ppEnabled()) {
    c.printLineOrdinary(tr("Post-processing subtitles file..."));
    napi.postProcessSubtitles();
  }

  c.printLineOrdinary(tr("Adjusting subtitles..."));
  if (!napi.matchSubtitles()) {
    c.printLineError(tr("Could not adjust subtitles!"));
    return EC_COULD_NOT_MATCH;
  }

  napi.cleanup();
  return 0;
}

int downloadForMovie(const Console& c, const QString& movieFilePath, int i,
                     int total, const QNapiConfig& config, QNapi& napi) {
  QString movieFileName = QFileInfo(movieFilePath).fileName();
  c.printLineHighlihted(tr("Downloading subtitles for '%1' [%2/%3]")
                            .arg(movieFileName)
                            .arg(i)
                            .arg(total));

  napi.setMoviePath(movieFilePath);

  if (!napi.checkWritePermissions()) {
    c.printLineError(tr("No permission to write to the directory '%1'!")
                         .arg(QFileInfo(movieFilePath).path()));
    return EC_NO_WRITE_PERMISSIONS;
  }

  napi.clearSubtitlesList();

  bool found = findSubtitles(c, config, napi);

  if (!found) {
    c.printLineWarning(tr("Subtitles not found!"));
    return EC_SUBTITLES_NOT_FOUND;
  }

  Maybe<int> selIdx = selectSubtitles(c, config, napi);

  if (selIdx) {
    return finishSubtitles(selIdx.value(), c, napi);
  }

  napi.cleanup();

  return EC_OK;
}

int downloadSubtitlesFor(const Console& c, const QStringList& movieFilePaths,
                         const QNapiConfig& config) {
  int ccErr = configChecks(c, config);
  if (ccErr != 0) {
    return ccErr;
  }

  QNapi napi(config);

  int total = movieFilePaths.size();
  for (int i = 1; i <= total; ++i) {
    QString movieFilePath = movieFilePaths[i - 1];

    int result = downloadForMovie(c, movieFilePath, i, total, config, napi);
    if (result == EC_P7ZIP_UNAVAILABLE || result == EC_CANNOT_WRITE_TMP_DIR) {
      if (i < total) {
        c.printLineOrdinary(
            tr("Processing of remaining %n file(s) was ignored "
               "due to critical error.",
               "", total - i));
      }
      return result;
    }
  }

  return EC_OK;
}
};  // namespace CliSubtitlesDownloader

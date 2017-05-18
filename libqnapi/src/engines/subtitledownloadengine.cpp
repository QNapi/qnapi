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

#include "engines/subtitledownloadengine.h"
#include <QDir>
#include <QFlags>
#include "subconvert/subtitleformatsregistry.h"

SubtitleDownloadEngine::SubtitleDownloadEngine(const QString& tmpPath)
    : tmpPath(tmpPath) {}

void SubtitleDownloadEngine::setMoviePath(const QString& path) { movie = path; }

QString SubtitleDownloadEngine::moviePath() { return movie; }

void SubtitleDownloadEngine::clearSubtitlesList() {
  foreach (SubtitleInfo s, subtitlesList) {
    if (QFile::exists(s.sourceLocation)) QFile::remove(s.sourceLocation);
  }
  subtitlesList.clear();
}

Maybe<SubtitleInfo> SubtitleDownloadEngine::resolveById(QUuid id) {
  foreach (SubtitleInfo s, subtitlesList) {
    if (s.id == id) return just(s);
  }
  return nothing();
}

void SubtitleDownloadEngine::updateSubtitleInfo(const SubtitleInfo& si) {
  for (int i = 0; i < subtitlesList.size(); ++i) {
    if (subtitlesList[i].id == si.id) {
      subtitlesList[i] = si;
      break;
    }
  }
}

QString SubtitleDownloadEngine::generateTmpFileName() const {
  static bool gen_inited;
  if (!gen_inited) {
    qsrand(time(0));
    gen_inited = true;
  }
  return QString("QNapi.%1.tmp").arg(qrand());
}

QString SubtitleDownloadEngine::generateTmpPath() const {
  QString newTmpFilePath =
      QString("%1/%2").arg(tmpPath).arg(generateTmpFileName());
  return QDir::toNativeSeparators(newTmpFilePath);
}

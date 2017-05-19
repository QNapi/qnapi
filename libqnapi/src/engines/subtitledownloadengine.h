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

#ifndef SUBTITLEDOWNLOADENGINE_H
#define SUBTITLEDOWNLOADENGINE_H

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QUrl>

#include <ctime>

#include <Maybe.h>

#include "subtitleinfo.h"

#include "engines/subtitledownloadenginemetadata.h"

class SubtitleDownloadEngine {
 public:
  virtual ~SubtitleDownloadEngine() {}

  void setMoviePath(const QString& path);
  QString moviePath();

  virtual SubtitleDownloadEngineMetadata meta() const = 0;
  virtual const char* const* enginePixmapData() const = 0;

  virtual void clearSubtitlesList();
  virtual QString checksum(QString filename = "") = 0;
  virtual bool lookForSubtitles(QString lang) = 0;
  virtual QList<SubtitleInfo> listSubtitles() = 0;
  virtual bool download(QUuid id) = 0;
  virtual bool unpack(QUuid id) = 0;
  virtual void cleanup() = 0;

 protected:
  QString tmpPath;

  QList<SubtitleInfo> subtitlesList;
  QString movie;
  QString subtitles;
  QString subtitlesTmp;
  QString checkSum;

  SubtitleDownloadEngine(const QString& tmpPath);

  Maybe<SubtitleInfo> resolveById(QUuid id);
  void updateSubtitleInfo(const SubtitleInfo& si);

  QString generateTmpFileName() const;
  QString generateTmpPath() const;

  friend class QNapi;
};

#endif

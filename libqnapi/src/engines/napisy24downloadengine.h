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

#ifndef NAPISY24DOWNLOADENGINE_H
#define NAPISY24DOWNLOADENGINE_H

#include "config/engineconfig.h"
#include "config/staticconfig.h"
#include "engines/subtitledownloadengine.h"
#include "utils/p7zipdecoder.h"
#include "utils/synchttp.h"

#include <QSharedPointer>

class Napisy24DownloadEngine : public SubtitleDownloadEngine {
 public:
  Napisy24DownloadEngine(const QString& tmpPath, const EngineConfig& config,
                         const QSharedPointer<const P7ZipDecoder>& p7zipDecoder,
                         const QStringList& subtitleExtensions);
  ~Napisy24DownloadEngine();

  static SubtitleDownloadEngineMetadata metadata;
  static const char* const pixmapData[];

  SubtitleDownloadEngineMetadata meta() const;
  const char* const* enginePixmapData() const;

  QString checksum(QString filename = "");
  bool lookForSubtitles(QString lang);
  QList<SubtitleInfo> listSubtitles();
  bool download(QUuid id);
  bool unpack(QUuid id);
  void cleanup();

 private:
  QPair<QString, QString> getCredentials() const;

  EngineConfig engineConfig;
  QSharedPointer<const P7ZipDecoder> p7zipDecoder;
  QStringList subtitleExtensions;

  quint64 fileSize;
};

#endif  // NAPISY24DOWNLOADENGINE_H

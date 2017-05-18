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

#ifndef SUBTITLEDOWNLOADENGINESREGISTRY_H
#define SUBTITLEDOWNLOADENGINESREGISTRY_H

#include "config/qnapiconfig.h"

#include "engines/subtitledownloadengine.h"

#include "utils/p7zipdecoder.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <functional>

class SubtitleDownloadEnginesRegistry {
 public:
  SubtitleDownloadEnginesRegistry(
      const QString &qnapiDisplayableVersion,
      const QStringList &subtitleExtensions,
      std::function<QSharedPointer<const P7ZipDecoder>(const QString &)>
          p7zipDecoderFactory);

  QStringList listEngineNames() const;
  SubtitleDownloadEngineMetadata engineMetadata(
      const QString &engineName) const;
  const char *const *enginePixmapData(const QString &engineName) const;

  QSharedPointer<SubtitleDownloadEngine> createEngine(
      const QString &engineName, const QNapiConfig &config) const;
  QList<QSharedPointer<SubtitleDownloadEngine>> createEngines(
      const QStringList &engineNames, const QNapiConfig &config) const;
  QList<QSharedPointer<SubtitleDownloadEngine>> createEnabledEngines(
      const QNapiConfig &config) const;

 private:
  const QString qnapiDisplayableVersion;
  const QStringList subtitleExtensions;
  std::function<QSharedPointer<const P7ZipDecoder>(const QString &)>
      p7zipDecoderFactory;
};

#endif  // SUBTITLEDOWNLOADENGINESREGISTRY_H

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

#include "subtitledownloadenginesregistry.h"

#include "engines/napiprojektdownloadengine.h"
#include "engines/napisy24downloadengine.h"
#include "engines/opensubtitlesdownloadengine.h"

#include "utils/p7zipdecoder.h"

#include <Maybe.h>

#include <QPair>

SubtitleDownloadEnginesRegistry::SubtitleDownloadEnginesRegistry(
    const QString &qnapiDisplayableVersion,
    const QStringList &subtitleExtensions,
    std::function<QSharedPointer<const P7ZipDecoder>(const QString &)>
        p7zipDecoderFactory)
    : qnapiDisplayableVersion(qnapiDisplayableVersion),
      subtitleExtensions(subtitleExtensions),
      p7zipDecoderFactory(p7zipDecoderFactory) {}

QStringList SubtitleDownloadEnginesRegistry::listEngineNames() const {
  static QStringList engineNames = {
      NapiProjektDownloadEngine::metadata.name(),
      OpenSubtitlesDownloadEngine::metadata.name(),
      Napisy24DownloadEngine::metadata.name()};
  return engineNames;
}

SubtitleDownloadEngineMetadata SubtitleDownloadEnginesRegistry::engineMetadata(
    const QString &engineName) const {
  if (engineName == NapiProjektDownloadEngine::metadata.name())
    return NapiProjektDownloadEngine::metadata;

  if (engineName == OpenSubtitlesDownloadEngine::metadata.name())
    return OpenSubtitlesDownloadEngine::metadata;

  if (engineName == Napisy24DownloadEngine::metadata.name())
    return Napisy24DownloadEngine::metadata;

  return SubtitleDownloadEngineMetadata("", "", nothing(), nothing());
}

const char *const *SubtitleDownloadEnginesRegistry::enginePixmapData(
    const QString &engineName) const {
  if (engineName == NapiProjektDownloadEngine::metadata.name())
    return NapiProjektDownloadEngine::pixmapData;

  if (engineName == OpenSubtitlesDownloadEngine::metadata.name())
    return OpenSubtitlesDownloadEngine::pixmapData;

  if (engineName == Napisy24DownloadEngine::metadata.name())
    return Napisy24DownloadEngine::pixmapData;

  return {};
}

QSharedPointer<SubtitleDownloadEngine>
SubtitleDownloadEnginesRegistry::createEngine(const QString &engineName,
                                              const QNapiConfig &config) const {
  QSharedPointer<const P7ZipDecoder> p7zipDecoder =
      p7zipDecoderFactory(config.generalConfig().p7zipPath());
  const QString &tmpPath = config.generalConfig().tmpPath();

  if (engineName == NapiProjektDownloadEngine::metadata.name()) {
    const EngineConfig &engineConfig = config.enginesConfig().value(
        NapiProjektDownloadEngine::metadata.name(), EngineConfig::empty);
    return QSharedPointer<SubtitleDownloadEngine>(
        new NapiProjektDownloadEngine(tmpPath, engineConfig, p7zipDecoder));
  }

  if (engineName == OpenSubtitlesDownloadEngine::metadata.name()) {
    const EngineConfig &engineConfig = config.enginesConfig().value(
        OpenSubtitlesDownloadEngine::metadata.name(), EngineConfig::empty);
    const QString &language = config.generalConfig().language();
    return QSharedPointer<SubtitleDownloadEngine>(
        new OpenSubtitlesDownloadEngine(tmpPath, engineConfig, p7zipDecoder,
                                        qnapiDisplayableVersion, language));
  }

  if (engineName == Napisy24DownloadEngine::metadata.name()) {
    const EngineConfig &engineConfig = config.enginesConfig().value(
        Napisy24DownloadEngine::metadata.name(), EngineConfig::empty);
    return QSharedPointer<SubtitleDownloadEngine>(new Napisy24DownloadEngine(
        tmpPath, engineConfig, p7zipDecoder, subtitleExtensions));
  }

  return QSharedPointer<SubtitleDownloadEngine>();
}

QList<QSharedPointer<SubtitleDownloadEngine>>
SubtitleDownloadEnginesRegistry::createEngines(
    const QStringList &engineNames, const QNapiConfig &config) const {
  QList<QSharedPointer<SubtitleDownloadEngine>> engines;
  foreach (QString engineName, engineNames) {
    engines << createEngine(engineName, config);
  }
  return engines;
}

QList<QSharedPointer<SubtitleDownloadEngine>>
SubtitleDownloadEnginesRegistry::createEnabledEngines(
    const QNapiConfig &config) const {
  QStringList engineNames;
  typedef QPair<QString, bool> EngineEnableCfg;

  foreach (EngineEnableCfg engineCfg, config.enabledEngines()) {
    if (engineCfg.second) engineNames << engineCfg.first;
  }

  return createEngines(engineNames, config);
}

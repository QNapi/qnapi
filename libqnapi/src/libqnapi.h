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

#ifndef LIBQNAPI_H
#define LIBQNAPI_H

#include "config/configreader.h"
#include "config/configwriter.h"
#include "config/qnapiconfig.h"
#include "config/staticconfig.h"

#include "engines/subtitledownloadenginesregistry.h"

#include "movieinfo/movieinfoprovider.h"

#include "subconvert/subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"

#include "utils/p7zipdecoder.h"

#include "subtitlematcher.h"
#include "subtitlepostprocessor.h"

#include <QSharedPointer>
#include <QString>

class LibQNapi {
 public:
  LibQNapi();

  static QString appExecutableFilePath;
  static QString appExecutableDir;

  static void init(const QString& appExecutablePath);

  static QString version();
  static QString displayableVersion();
  static QString webpageUrl();

  static QSharedPointer<const StaticConfig> staticConfig();
  static QSharedPointer<const ConfigReader> configReader();
  static QSharedPointer<const ConfigWriter> configWriter();

  static const QNapiConfig loadConfig();
  static void writeConfig(const QNapiConfig& config);
  static bool isPortableMode();
  static QString portableConfigPath();

  static QString uiLanguage(const GeneralConfig& generalConfig);

  static QSharedPointer<const SubtitleDownloadEnginesRegistry>
  subtitleDownloadEngineRegistry();

  static QSharedPointer<const MovieInfoProvider> movieInfoProvider();

  static QSharedPointer<const SubtitleFormatsRegistry>
  subtitleFormatsRegistry();
  static QSharedPointer<const SubtitleConverter> subtitleConverter(
      const PostProcessingConfig& ppConfig);

  static QSharedPointer<const P7ZipDecoder> p7zipDecoder(
      const QString& p7zipPath);

  static QSharedPointer<const SubtitleMatcher> subtitleMatcher(
      const QNapiConfig& config);
  static QSharedPointer<const SubtitlePostProcessor> subtitlePostProcessor(
      const PostProcessingConfig& config);
};

#endif  // LIBQNAPI_H

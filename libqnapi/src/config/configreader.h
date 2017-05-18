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

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QMap>
#include <QSettings>
#include <QSharedPointer>
#include <QString>

#include <config/engineconfig.h>
#include <config/qnapiconfig.h>
#include <config/staticconfig.h>

#include <engines/subtitledownloadenginesregistry.h>

class ConfigReader {
 public:
  ConfigReader(const QString& appExecutableDir,
               const QSharedPointer<const StaticConfig>& staticConfig,
               const QSharedPointer<const SubtitleDownloadEnginesRegistry>
                   enginesRegistry);

  const QNapiConfig readUserConfig() const;
  const QNapiConfig readPortableConfig(const QString& configFilePath) const;
  const QNapiConfig readConfig(const QSettings& settings) const;

 private:
  const GeneralConfig readGeneralConfig(const QSettings& settings) const;
  const QList<QPair<QString, bool>> readEnabledEngines(
      const QSettings& settings) const;
  const QMap<QString, EngineConfig> readEnginesConfig(
      const QSettings& settings) const;
  const EngineConfig readEngineConfig(QString engineName,
                                      const QSettings& settings) const;
  const PostProcessingConfig readPostProcessingConfig(
      const QSettings& settings) const;
  const ScanConfig readScanConfig(const QSettings& settings) const;

  const GeneralConfig resolveP7zipPath(const GeneralConfig& config) const;
  const GeneralConfig resolveTmpPath(const GeneralConfig& config) const;

  QString appExecutableDir;
  QSharedPointer<const StaticConfig> staticConfig;
  QSharedPointer<const SubtitleDownloadEnginesRegistry> enginesRegistry;
};

#endif  // CONFIGREADER_H

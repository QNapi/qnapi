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

#ifndef CONFIGWRITER_H
#define CONFIGWRITER_H

#include <QMap>
#include <QSettings>
#include <QString>

#include <config/engineconfig.h>
#include <config/qnapiconfig.h>

class ConfigWriter {
 public:
  ConfigWriter(const QString& qnapiVersion);

  void writeUserConfig(const QNapiConfig& config) const;
  void writePortableConfig(const QString& configFilePath,
                           const QNapiConfig& config) const;
  void writeConfig(const QNapiConfig& config, QSettings& settings) const;

 private:
  void writeGeneralConfig(const GeneralConfig& generalConfig,
                          QSettings& settings) const;
  void writeEnabledEngines(const QList<QPair<QString, bool>>& enabledEngines,
                           QSettings& settings) const;
  void writeEnginesConfig(const QMap<QString, EngineConfig>& enginesConfig,
                          QSettings& settings) const;
  void writeEngineConfig(QString engineName, const EngineConfig& engineConfig,
                         QSettings& settings) const;
  void writePostProcessingConfig(
      const PostProcessingConfig& postProcessingConfig,
      QSettings& settings) const;
  void writeScanConfig(const ScanConfig& scanConfig, QSettings& settings) const;

  QString qnapiVersion;
};

#endif  // CONFIGWRITER_H

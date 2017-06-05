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

#ifndef QNAPICONFIG_H
#define QNAPICONFIG_H

#include <config/engineconfig.h>
#include <config/generalconfig.h>
#include <config/postprocessingconfig.h>
#include <config/scanconfig.h>
#include <QMap>
#include <QString>

class QNapiConfig {
 private:
  bool firstrun_;
  QString version_;
  GeneralConfig generalConfig_;
  QList<QPair<QString, bool>> enabledEngines_;
  QMap<QString, EngineConfig> enginesConfig_;
  PostProcessingConfig postProcessingConfig_;
  ScanConfig scanConfig_;
  QString lastOpenedDir_;

 public:
  QNapiConfig(){};
  QNapiConfig(const bool& firstrun, const QString& version,
              const GeneralConfig& generalConfig,
              const QList<QPair<QString, bool>>& enabledEngines,
              const QMap<QString, EngineConfig>& enginesConfig,
              const PostProcessingConfig& postProcessingConfig,
              const ScanConfig& scanConfig, const QString& lastOpenedDir)
      : firstrun_(firstrun),
        version_(version),
        generalConfig_(generalConfig),
        enabledEngines_(enabledEngines),
        enginesConfig_(enginesConfig),
        postProcessingConfig_(postProcessingConfig),
        scanConfig_(scanConfig),
        lastOpenedDir_(lastOpenedDir) {}

  bool firstrun() const { return firstrun_; }
  QString version() const { return version_; }
  GeneralConfig generalConfig() const { return generalConfig_; }
  QList<QPair<QString, bool>> enabledEngines() const { return enabledEngines_; }
  QMap<QString, EngineConfig> enginesConfig() const { return enginesConfig_; }
  PostProcessingConfig postProcessingConfig() const {
    return postProcessingConfig_;
  }
  ScanConfig scanConfig() const { return scanConfig_; }
  QString lastOpenedDir() const { return lastOpenedDir_; }

  const QNapiConfig setFirstrun(const bool& firstrun) const {
    return QNapiConfig(firstrun, version_, generalConfig_, enabledEngines_,
                       enginesConfig_, postProcessingConfig_, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setVersion(const QString& version) const {
    return QNapiConfig(firstrun_, version, generalConfig_, enabledEngines_,
                       enginesConfig_, postProcessingConfig_, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setGeneralConfig(const GeneralConfig& generalConfig) const {
    return QNapiConfig(firstrun_, version_, generalConfig, enabledEngines_,
                       enginesConfig_, postProcessingConfig_, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setEnabledEngines(
      const QList<QPair<QString, bool>>& enabledEngines) const {
    return QNapiConfig(firstrun_, version_, generalConfig_, enabledEngines,
                       enginesConfig_, postProcessingConfig_, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setEnginesConfig(
      const QMap<QString, EngineConfig>& enginesConfig) const {
    return QNapiConfig(firstrun_, version_, generalConfig_, enabledEngines_,
                       enginesConfig, postProcessingConfig_, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setPostProcessingConfig(
      const PostProcessingConfig& postProcessingConfig) const {
    return QNapiConfig(firstrun_, version_, generalConfig_, enabledEngines_,
                       enginesConfig_, postProcessingConfig, scanConfig_,
                       lastOpenedDir_);
  }
  const QNapiConfig setScanConfig(const ScanConfig& scanConfig) const {
    return QNapiConfig(firstrun_, version_, generalConfig_, enabledEngines_,
                       enginesConfig_, postProcessingConfig_, scanConfig,
                       lastOpenedDir_);
  }
  const QNapiConfig setLastOpenedDir(const QString& lastOpenedDir) const {
    return QNapiConfig(firstrun_, version_, generalConfig_, enabledEngines_,
                       enginesConfig_, postProcessingConfig_, scanConfig_,
                       lastOpenedDir);
  }

  QString toString() const;
};

#endif

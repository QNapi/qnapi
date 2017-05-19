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

#include "configwriter.h"
#include <QTextStream>

ConfigWriter::ConfigWriter(const QString& qnapiVersion)
    : qnapiVersion(qnapiVersion) {}

void ConfigWriter::writeUserConfig(const QNapiConfig& config) const {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
  writeConfig(config, settings);
}

void ConfigWriter::writePortableConfig(const QString& configFilePath,
                                       const QNapiConfig& config) const {
  QSettings settings(configFilePath, QSettings::IniFormat);
  writeConfig(config, settings);
}

void ConfigWriter::writeConfig(const QNapiConfig& config,
                               QSettings& settings) const {
  settings.setValue("qnapi/firstrun", false);
  settings.setValue("qnapi/version", qnapiVersion);
  writeGeneralConfig(config.generalConfig(), settings);
  writeEnabledEngines(config.enabledEngines(), settings);
  writeEnginesConfig(config.enginesConfig(), settings);
  writePostProcessingConfig(config.postProcessingConfig(), settings);
  writeScanConfig(config.scanConfig(), settings);
  settings.setValue("qnapi/last_opened_dir", config.lastOpenedDir());
  settings.sync();
}

void ConfigWriter::writeGeneralConfig(const GeneralConfig& generalConfig,
                                      QSettings& settings) const {
  settings.setValue("qnapi/ui_language", generalConfig.uiLanguage());
  settings.setValue("qnapi/7z_path", generalConfig.p7zipPath());
  settings.setValue("qnapi/tmp_path", generalConfig.tmpPath());
  settings.setValue("qnapi/language", generalConfig.language());
  settings.setValue("qnapi/language_backup", generalConfig.backupLanguage());
  settings.setValue("qnapi/no_backup", generalConfig.noBackup());
  settings.setValue("qnapi/quiet_batch", generalConfig.quietBatch());
  settings.setValue("qnapi/search_policy", generalConfig.searchPolicy());
  settings.setValue("qnapi/download_policy", generalConfig.downloadPolicy());
  settings.setValue("qnapi/change_permissions",
                    generalConfig.changePermissionsEnabled());
  settings.setValue("qnapi/permissions", generalConfig.changePermissionsTo());
}

void ConfigWriter::writeEnabledEngines(
    const QList<QPair<QString, bool>>& enabledEngines,
    QSettings& settings) const {
  QStringList enabledEnginesStr;
  typedef QPair<QString, bool> EngineEnableCfg;
  foreach (EngineEnableCfg engineCfg, enabledEngines) {
    QString engineName = engineCfg.first;
    QString engineEnableStr = engineCfg.second ? "on" : "off";
    enabledEnginesStr << engineName + ":" + engineEnableStr;
  }

  settings.setValue("qnapi/engines", enabledEnginesStr);
}

void ConfigWriter::writeEnginesConfig(
    const QMap<QString, EngineConfig>& enginesConfig,
    QSettings& settings) const {
  foreach (QString engineName, enginesConfig.keys()) {
    const EngineConfig& engineConfig = *enginesConfig.find(engineName);
    writeEngineConfig(engineName, engineConfig, settings);
  }
}

void ConfigWriter::writeEngineConfig(QString engineName,
                                     const EngineConfig& engineConfig,
                                     QSettings& settings) const {
  settings.setValue(engineName + "/nick", engineConfig.nick());
  settings.setValue(engineName + "/password", engineConfig.password());
}

void ConfigWriter::writePostProcessingConfig(
    const PostProcessingConfig& postProcessingConfig,
    QSettings& settings) const {
  settings.setValue("qnapi/post_processing", postProcessingConfig.enabled());
  settings.setValue("qnapi/encoding_method",
                    postProcessingConfig.encodingChangeMethod());
  settings.setValue("qnapi/enc_from", postProcessingConfig.encodingFrom());
  settings.setValue("qnapi/auto_detect_encoding",
                    postProcessingConfig.encodingAutoDetectFrom());
  settings.setValue("qnapi/enc_to", postProcessingConfig.encodingTo());
  settings.setValue("qnapi/show_all_encodings",
                    postProcessingConfig.showAllEncodings());
  settings.setValue("qnapi/sub_format", postProcessingConfig.subFormat());
  settings.setValue("qnapi/sub_ext", postProcessingConfig.subExtension());
  settings.setValue("qnapi/skip_convert_ads",
                    postProcessingConfig.skipConvertAds());
  settings.setValue("qnapi/remove_words", postProcessingConfig.removeWords());
}

void ConfigWriter::writeScanConfig(const ScanConfig& scanConfig,
                                   QSettings& settings) const {
  settings.setValue("scan/last_scan_dir", scanConfig.lastDir());
  settings.setValue("scan/skip_if_subtitles_exist",
                    scanConfig.skipIfSubtitlesExist());
  settings.setValue("scan/filters", scanConfig.filters());
  settings.setValue("scan/skip_filters", scanConfig.skipFilters());
}

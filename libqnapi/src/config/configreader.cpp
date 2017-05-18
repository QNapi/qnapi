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

#include "configreader.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include "subtitlelanguage.h"

ConfigReader::ConfigReader(
    const QString& appExecutableDir,
    const QSharedPointer<const StaticConfig>& staticConfig,
    const QSharedPointer<const SubtitleDownloadEnginesRegistry> enginesRegistry)
    : appExecutableDir(appExecutableDir),
      staticConfig(staticConfig),
      enginesRegistry(enginesRegistry) {}

const QNapiConfig ConfigReader::readUserConfig() const {
  return readConfig(
      QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi"));
}

const QNapiConfig ConfigReader::readPortableConfig(
    const QString& configFilePath) const {
  return readConfig(QSettings(configFilePath, QSettings::IniFormat));
}

const QNapiConfig ConfigReader::readConfig(const QSettings& settings) const {
  return QNapiConfig(settings.value("qnapi/firstrun", true).toBool(),
                     settings.value("qnapi/version", "").toString(),
                     readGeneralConfig(settings), readEnabledEngines(settings),
                     readEnginesConfig(settings),
                     readPostProcessingConfig(settings),
                     readScanConfig(settings),
                     settings.value("qnapi/last_opened_dir", "").toString());
}

const GeneralConfig ConfigReader::readGeneralConfig(
    const QSettings& settings) const {
  QString sysLangCode = QLocale::system().name().left(2);
  QString preferredLangCode =
      SubtitleLanguage().listLanguageTwoLetterCodes().contains(sysLangCode)
          ? sysLangCode
          : "en";
  QString backupLangCode = (preferredLangCode != "en") ? "en" : "";

  auto cfg = GeneralConfig(
      settings.value("qnapi/ui_language", "").toString(),
      settings.value("qnapi/7z_path", "").toString(),
      settings.value("qnapi/tmp_path", "").toString(),
      settings.value("qnapi/language", preferredLangCode).toString(),
      settings.value("qnapi/language_backup", backupLangCode).toString(),
      settings.value("qnapi/no_backup", false).toBool(),
      settings.value("qnapi/quiet_batch", false).toBool(),
      static_cast<SearchPolicy>(
          settings.value("qnapi/search_policy", SP_BREAK_IF_FOUND).toInt()),
      static_cast<DownloadPolicy>(
          settings.value("qnapi/download_policy", DP_SHOW_LIST_IF_NEEDED)
              .toInt()),
      settings.value("qnapi/change_permissions", false).toBool(),
      settings.value("qnapi/permissions", "644").toString());

  auto cfg1 = resolveP7zipPath(cfg);
  auto cfg2 = resolveTmpPath(cfg1);
  return cfg2;
}

const QList<QPair<QString, bool>> ConfigReader::readEnabledEngines(
    const QSettings& settings) const {
  QList<QPair<QString, bool>> defaultEnabledEngines;
  foreach (QString engineName, enginesRegistry->listEngineNames()) {
    defaultEnabledEngines << qMakePair(engineName, true);
  }

  QStringList enabledEnginesStr =
      settings.value("qnapi/engines", QStringList()).toStringList();

  if (enabledEnginesStr.size() != enginesRegistry->listEngineNames().size()) {
    return defaultEnabledEngines;
  } else {
    QList<QPair<QString, bool>> enabledEngines;
    foreach (QString engineEnableStr, enabledEnginesStr) {
      QStringList engineParts =
          engineEnableStr.split(":", QString::SkipEmptyParts);
      if (engineParts.size() != 2) {
        return defaultEnabledEngines;
      }
      enabledEngines << qMakePair(engineParts[0], engineParts[1] == "on");
    }
    return enabledEngines;
  }
}

const QMap<QString, EngineConfig> ConfigReader::readEnginesConfig(
    const QSettings& settings) const {
  QMap<QString, EngineConfig> engineConfigurations;
  foreach (QString engineName, enginesRegistry->listEngineNames()) {
    engineConfigurations.insert(engineName,
                                readEngineConfig(engineName, settings));
  }
  return engineConfigurations;
}

const EngineConfig ConfigReader::readEngineConfig(
    QString engineName, const QSettings& settings) const {
  return EngineConfig(settings.value(engineName + "/nick", "").toString(),
                      settings.value(engineName + "/password", "").toString());
}

const PostProcessingConfig ConfigReader::readPostProcessingConfig(
    const QSettings& settings) const {
  return PostProcessingConfig(
      settings.value("qnapi/post_processing", true).toBool(),
      static_cast<EncodingChangeMethod>(
          settings.value("qnapi/encoding_method", ECM_CHANGE).toInt()),
      settings.value("qnapi/enc_from", "windows-1250").toString(),
      settings.value("qnapi/auto_detect_encoding", true).toBool(),
      settings.value("qnapi/enc_to", "UTF-8").toString(),
      settings.value("qnapi/show_all_encodings", false).toBool(),
      settings.value("qnapi/sub_format", "srt").toString(),
      settings.value("qnapi/sub_ext", "").toString(),
      settings.value("qnapi/skip_convert_ads", false).toBool(),
      settings.value("qnapi/remove_lines", false).toBool(),
      settings
          .value("qnapi/remove_words", QStringList() << "movie info"
                                                     << "synchro")
          .toStringList());
}

const ScanConfig ConfigReader::readScanConfig(const QSettings& settings) const {
  return ScanConfig(
      settings.value("scan/last_scan_dir", "").toString(),
      settings.value("scan/skip_if_subtitles_exist", false).toBool(),
      settings.value("scan/filters", QStringList()).toStringList(),
      settings.value("scan/skip_filters", "PL dubbing").toString());
}

const GeneralConfig ConfigReader::resolveP7zipPath(
    const GeneralConfig& config) const {
  if (QFileInfo(config.p7zipPath()).isExecutable()) {
    return config;
  } else {
    QString p7zipPath = "";

#if defined(Q_OS_MAC)
    p7zipPath =
        QFileInfo(appExecutableDir + "/../Resources/7za").absoluteFilePath();
#elif defined(Q_OS_WIN)
    p7zipPath = QFileInfo(appExecutableDir + "/7za.exe").absoluteFilePath();
#else
    QString pathEnv =
        QProcess::systemEnvironment().filter(QRegExp("^PATH=(.*)$")).value(0);
    QStringList sysPaths = pathEnv.mid(5).split(":");
    sysPaths.removeAll("");
    sysPaths.append(".");

    if (sysPaths.isEmpty()) {
      sysPaths << "/bin"
               << "/usr/bin"
               << "/usr/local/bin";
    }

    sysPaths << appExecutableDir;

    QStringList p7zipBinaries = {"7z", "7za"};

    foreach (const QString sysPath, sysPaths) {
      foreach (const QString p7zipBinary, p7zipBinaries) {
        QFileInfo fi(sysPath + QDir::separator() + p7zipBinary);
        if (fi.isExecutable()) {
          p7zipPath = fi.absoluteFilePath();
          break;
        }
      }

      if (!p7zipPath.isEmpty()) {
        break;
      }
    }

    if (p7zipPath.isEmpty()) {
      p7zipPath = "7z";
    }
#endif

    return config.setP7zipPath(p7zipPath);
  }
}

const GeneralConfig ConfigReader::resolveTmpPath(
    const GeneralConfig& config) const {
  QFileInfo fi(config.tmpPath());

  if (fi.isDir() && fi.isWritable()) {
    return config;
  } else {
    return config.setTmpPath(QDir::tempPath());
  }
}

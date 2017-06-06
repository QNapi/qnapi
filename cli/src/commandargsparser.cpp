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

#include "commandargsparser.h"
#include "libqnapi.h"
#include "qnapiclicommand.h"
#include "subtitlelanguage.h"

#include <QFileInfo>

namespace CommandArgsParser {

using namespace QNapiCliCommand;

Either<QString, SubtitleLanguage> handleLangArgument(const QStringList& args,
                                                     int i) {
  if (i >= args.size()) {
    return some(tr("You need to pass language code after %1 argument")
                    .arg(args[i - 1]));
  }
  QString nextArg = args[i];
  SubtitleLanguage sl(nextArg);
  if (sl.toTwoLetter().isEmpty()) {
    return some(tr("Invalid language code passed: %1").arg(nextArg));
  } else {
    return some(sl);
  }
}

Maybe<QString> extractFilePath(const QString& arg) {
  if (QFileInfo(arg).isFile()) {
    return just(arg);
  }

  if (arg.startsWith("file://")) {
    QString withRemovedPrefix = arg;
    withRemovedPrefix.remove(0, 7);
    if (QFileInfo(withRemovedPrefix).isFile()) {
      return just(withRemovedPrefix);
    }
  }

  return nothing();
}

Either<QString, ParsedCommand> parse(const QStringList& args,
                                     const QNapiConfig& config) {
  if (args.contains("-h") || args.contains("--help")) {
    return some(ParsedCommand{config, QVariant::fromValue(ShowHelp())});
  }

  if (args.contains("-hl") || args.contains("--help-languages")) {
    return some(
        ParsedCommand{config, QVariant::fromValue(ShowHelpLanguages())});
  }

  QStringList filePaths;
  QNapiConfig refinedConfig = config;

  for (int i = 0; i < args.size(); ++i) {
    QString arg = args[i];

    if (arg == "-q" || arg == "--quiet") {
      refinedConfig = refinedConfig.setGeneralConfig(
          refinedConfig.generalConfig().setQuietBatch(true));
    } else if (arg == "-s" || arg == "--show-list") {
      refinedConfig = refinedConfig.setGeneralConfig(
          refinedConfig.generalConfig().setDownloadPolicy(
              DownloadPolicy::DP_ALWAYS_SHOW_LIST));
    } else if (arg == "-d" || arg == "--dont-show-list") {
      refinedConfig = refinedConfig.setGeneralConfig(
          refinedConfig.generalConfig().setDownloadPolicy(
              DownloadPolicy::DP_NEVER_SHOW_LIST));
    } else if (arg == "-l" || arg == "--lang") {
      ++i;
      auto langOrErr = handleLangArgument(args, i);
      if (langOrErr.is<QString>()) {
        return some(langOrErr.as<QString>());
      } else {
        refinedConfig = refinedConfig.setGeneralConfig(
            refinedConfig.generalConfig().setLanguage(
                langOrErr.as<SubtitleLanguage>().toTwoLetter()));
      }
    } else if (arg == "-lb" || arg == "--lang-backup") {
      ++i;
      auto langOrErr = handleLangArgument(args, i);
      if (langOrErr.is<QString>()) {
        return some(langOrErr.as<QString>());
      } else {
        refinedConfig = refinedConfig.setGeneralConfig(
            refinedConfig.generalConfig().setBackupLanguage(
                langOrErr.as<SubtitleLanguage>().toTwoLetter()));
      }
    } else if (arg == "-f" || arg == "--format") {
      ++i;
      if (i >= args.size()) {
        return some(
            tr("You need to pass subtitles format code after %1 argument")
                .arg(args[i - 1]));
      }
      QString format = args[i];
      if (LibQNapi::subtitleFormatsRegistry()->select(format).isNull()) {
        return some(
            tr("Invalid target subtitles format passed: %1").arg(format));
      } else {
        refinedConfig = refinedConfig.setPostProcessingConfig(
            refinedConfig.postProcessingConfig().setSubFormat(format));
      }
    } else if (arg == "-e" || arg == "--extension") {
      ++i;
      if (i >= args.size()) {
        return some(tr("You need to pass subtitles extension after %1 argument")
                        .arg(args[i - 1]));
      }
      QString ext = args[i];
      if (!QRegExp("^[A-Za-z0-9]+$").exactMatch(ext)) {
        return some(
            tr("Invalid target subtitles extension passed: %1").arg(ext));
      } else {
        refinedConfig = refinedConfig.setPostProcessingConfig(
            refinedConfig.postProcessingConfig().setSubExtension(ext));
      }
    } else {
      auto maybePath = extractFilePath(arg);
      if (maybePath) {
        filePaths << maybePath.value();
      } else {
        return some(tr("Invalid command line argument passed: %1").arg(arg));
      }
    }
  }

  if (filePaths.size() >= 1) {
    return some(ParsedCommand{
        refinedConfig, QVariant::fromValue(DownloadSubtitles{filePaths})});
  }

  return some(ParsedCommand{config, QVariant::fromValue(ShowHelp())});
}

}  // namespace CommandArgsParser

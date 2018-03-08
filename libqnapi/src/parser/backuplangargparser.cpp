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

#include "backuplangargparser.h"
#include "subtitlelanguage.h"

BackupLangArgParser::BackupLangArgParser() {}

QVariant BackupLangArgParser::parse(const QStringList& args,
                                    const QNapiConfig& config) const {
  int idx = args.indexOf("-lb");
  if (idx == -1) {
    idx = args.indexOf("--lang-backup");
  }
  if (idx == -1) {
    return QVariant::fromValue(NothingParsed());
  }

  if (idx == args.size() - 1) {
    return QVariant::fromValue(
        ParseError{tr("You need to pass backup language code after %1 argument")
                       .arg(args[idx])});
  }

  QString nextArg = args[idx + 1];
  SubtitleLanguage sl(nextArg);
  QString twoLetterLang = sl.toTwoLetter();

  if (twoLetterLang.isEmpty()) {
    return QVariant::fromValue(
        ParseError{tr("Invalid backup language code passed: %1").arg(nextArg)});
  } else {
    return QVariant::fromValue(ParsedModifier{config.setGeneralConfig(
        config.generalConfig().setBackupLanguage(twoLetterLang))});
  }
}

Maybe<CliArgParser::HelpInfo> BackupLangArgParser::helpInfo() const {
  return just(HelpInfo{"-lb", "--lang-backup", tr("code"),
                       tr("Alternative subtitles language")});
}

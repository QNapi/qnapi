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

#ifndef __QNAPICLI__H__
#define __QNAPICLI__H__

#include "config/qnapiconfig.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <iostream>
#include "qnapi.h"

class QNapiCli : public QCoreApplication {
  Q_OBJECT

 public:
  QNapiCli(int argc, char **argv, const QNapiConfig &config,
           const QString &uiLanguage);

  static bool isCliCall(int argc, char **argv);
  int exec();

 private:
  QStringList movieList;
  QNapiConfig config;
  const QString uiLanguage;

  enum CliExitCode {
    EC_CMD_LINE_ARG_PARSING_ERROR = -1,
    EC_OK = 0,
    EC_NON_CLI_MODE = 1,
    EC_P7ZIP_UNAVAILABLE = 2,
    EC_CANNOT_WRITE_TMP_DIR = 3,
    EC_UNSUPPORTED_ENGINE = 4,
    EC_NO_WRITE_PERMISSIONS = 5,
    EC_SUBTITLES_NOT_FOUND = 6,
    EC_COULD_NOT_DOWNLOAD = 7,
    EC_COULD_NOT_UNARCHIVE = 8,
    EC_COULD_NOT_MATCH = 9,
    EC_MOVIE_FILE_NOT_FOUND = 10
  };

  enum CliMode { CM_UNSET, CM_HELP, CM_HELP_LANGUAGES, CM_CONSOLE, CM_QUIET };

  enum ShowListPolicy { SLP_SHOW, SLP_USE_CONFIG, SLP_NEVER_SHOW };

  CliMode mode;
  ShowListPolicy showPolicy;

  QString lang;
  QString langBackup;
  bool langBackupPassed;
  QStringList args;

  bool analyze(const QStringList &args);
  void printHeader();
  void printHelp(const QString &binaryFileName);
  void printHelpLanguages();
  void printCli(const QString &string = "");
};

#endif

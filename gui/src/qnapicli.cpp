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

#include "qnapicli.h"
#include "libqnapi.h"
#include "subtitlelanguage.h"

#include <QTranslator>

QNapiCli::QNapiCli(int argc, char **argv, const QNapiConfig &config,
                   const QString &uiLanguage)
    : QCoreApplication(argc, argv),
      config(config),
      uiLanguage(uiLanguage),
      mode(CM_UNSET),
      showPolicy(SLP_USE_CONFIG),
      langBackupPassed(false) {
  args = arguments();
}

bool QNapiCli::isCliCall(int argc, char **argv) {
  QString p;

#ifdef Q_OS_WIN
  for (int i = 1; i < argc; i++) {
    p = argv[i];
    if ((p == "--quiet") || (p == "-q")) return true;
  }
  return false;
#else
  for (int i = 1; i < argc; i++) {
    p = argv[i];

    if (p == "--options") {
      return false;
    }

    if ((p == "--console") || (p == "-c") || (p == "--quiet") || (p == "-q")) {
      return true;
    }

    if ((p == "--help") || (p == "-h") || (p == "--help-languages") ||
        (p == "-hl")) {
      return true;
    }
  }

#if defined(Q_OS_MAC)
  return false;
#elif (defined(Q_OS_UNIX) || defined(Q_OS_LINUX))
  if (getenv("DISPLAY") == 0) return true;
  return false;
#else
  return false;
#endif
#endif
}

bool QNapiCli::analyze(const QStringList &args) {
  QString p;

  for (int i = 1; i < args.size(); i++) {
    p = args[i];

    if ((p == "--help") || (p == "-h")) {
      mode = CM_HELP;
      return true;
    } else if ((p == "--help-languages") || (p == "-hl")) {
      mode = CM_HELP_LANGUAGES;
      return true;
    } else if ((p == "--console") || (p == "-c")) {
      if (mode == CM_UNSET) {
#ifdef Q_OS_WIN
        mode = CM_QUIET;
        showPolicy = SLP_NEVER_SHOW;
#else
        mode = CM_CONSOLE;
#endif
      }
    } else if ((p == "--quiet") || (p == "-q")) {
      mode = CM_QUIET;
      showPolicy = SLP_NEVER_SHOW;
    } else if ((p == "--lang") || (p == "-l")) {
      ++i;
      if (i >= args.size()) return false;
      p = args[i];

      lang = SubtitleLanguage(p).toTwoLetter();
      if (lang.isEmpty()) {
        printCli(tr("Invalid language code: %1").arg(p));
        return false;
      }
    } else if ((p == "--lang-backup") || (p == "-lb")) {
      ++i;
      if (i >= args.size()) return false;
      p = args[i];

      langBackup = SubtitleLanguage(p).toTwoLetter();
      langBackupPassed = true;
    } else if ((p == "--format") || (p == "-f")) {
      ++i;
      if (i >= args.size()) return false;
      p = args[i];

      if (LibQNapi::subtitleFormatsRegistry()->select(p).isNull()) {
        printCli(tr("Invalid target subtitles format: %1").arg(p));
        return false;
      }

      config = config.setPostProcessingConfig(
          config.postProcessingConfig().setSubFormat(p));
    } else if ((p == "--extension") || (p == "-e")) {
      ++i;
      if (i >= args.size()) return false;
      p = args[i];

      config = config.setPostProcessingConfig(
          config.postProcessingConfig().setSubExtension(p));
    } else if ((p == "--show-list") || (p == "-s")) {
      if (mode != CM_QUIET) showPolicy = SLP_SHOW;
    } else if ((p == "--dont-show-list") || (p == "-d")) {
      showPolicy = SLP_NEVER_SHOW;
    } else if (QFileInfo(p).isFile()) {
      movieList << p;
    }
  }

  if (config.generalConfig().quietBatch() && !movieList.isEmpty()) {
    mode = CM_QUIET;
    return true;
  }

#if (defined(Q_OS_UNIX) || defined(Q_OS_LINUX))
  if ((getenv("DISPLAY") == 0) && (mode == CM_UNSET)) mode = CM_CONSOLE;
#endif

  return true;
}

int QNapiCli::exec() {
  QTranslator qnapiTranslator;
  qnapiTranslator.load("qnapi_" + uiLanguage, ":/translations");
  installTranslator(&qnapiTranslator);

  if (!analyze(args)) return EC_CMD_LINE_ARG_PARSING_ERROR;

  if (mode == CM_UNSET) return 1;

  if (mode != CM_QUIET) printHeader();

  if (mode == CM_HELP_LANGUAGES) {
    printHelpLanguages();
    return EC_OK;
  }

  if ((mode == CM_CONSOLE || mode == CM_QUIET) && (movieList.isEmpty())) {
    printCli(tr("File not found!"));
    return EC_MOVIE_FILE_NOT_FOUND;
  }

  if (mode == CM_HELP) {
    printHelp(QFileInfo(args.first()).fileName());
    return EC_OK;
  }

  QNapi napi(config);

  if (!napi.checkP7ZipPath()) {
    printCli(
        tr("Path to the program p7zip is incorrect! Check your settings."));
    return EC_P7ZIP_UNAVAILABLE;
  }

  if (!napi.checkTmpPath()) {
    printCli(
        tr("Can not write to the temporary directory! Check your settings."));
    return EC_CANNOT_WRITE_TMP_DIR;
  }

  if (lang.isEmpty()) lang = config.generalConfig().language();

  if (!langBackupPassed) langBackup = config.generalConfig().backupLanguage();

  foreach (QString movie, movieList) {
    printCli(tr(" * Downloading subtitles for '%1'")
                 .arg(QFileInfo(movie).fileName()));

    napi.setMoviePath(movie);

    if (!napi.checkWritePermissions()) {
      printCli(tr("   No permission to write to the directory '%1'!")
                   .arg(QFileInfo(movie).path()));
      return EC_NO_WRITE_PERMISSIONS;
    }

    napi.clearSubtitlesList();

    printCli(tr("   Calculating checksums..."));
    napi.checksum();

    bool found = false;
    SearchPolicy sp = config.generalConfig().searchPolicy();

    if (sp == SP_SEARCH_ALL_WITH_BACKUP_LANG) {
      foreach (QString e, napi.listLoadedEngines()) {
        printCli(tr("   Searching for subtitles [%1] (%2)...").arg(lang, e));
        found = napi.lookForSubtitles(lang, e) || found;
        printCli(tr("   Searching for subtitles in alternative language [%1] "
                    "(%2)...")
                     .arg(langBackup, e));
        found = napi.lookForSubtitles(langBackup, e) || found;
      }
    } else {
      foreach (QString e, napi.listLoadedEngines()) {
        printCli(tr("   Searching for subtitles [%1] (%2)...").arg(lang, e));
        found = napi.lookForSubtitles(lang, e) || found;

        if (sp == SP_BREAK_IF_FOUND && found) break;
      }

      if (!found && !langBackup.isEmpty()) {
        foreach (QString e, napi.listLoadedEngines()) {
          printCli(tr("   Searching for subtitles in alternative language [%1] "
                      "(%2)...")
                       .arg(langBackup, e));
          found = napi.lookForSubtitles(langBackup, e) || found;

          if (sp == SP_BREAK_IF_FOUND && found) break;
        }
      }
    }

    if (!found) {
      printCli(tr("   Subtitles not found!"));
      return EC_SUBTITLES_NOT_FOUND;
    }

    int selIdx = 0;

    bool showList = false;
    bool napiShowList = napi.needToShowList();

    if ((mode != CM_QUIET) && (showPolicy != SLP_NEVER_SHOW))
      showList = napiShowList;
    else if (showPolicy == SLP_SHOW)
      showList = true;

    // jesli mozna i potrzeba, listujemy dostepne napisy
    if (showList) {
      bool ok = false;

      printCli(tr("   0)\tDo not download subtitles for this video"));
      printCli(tr("   Found subtitles:"));

      int i = 1;

      QList<SubtitleInfo> list = napi.listSubtitles();

      foreach (SubtitleInfo s, list) {
        QString resolution = "";

        if (s.resolution == SUBTITLE_GOOD)
          resolution = tr(" (good)");
        else if (s.resolution == SUBTITLE_BAD)
          resolution = tr(" (bad)");

        printCli(QString("   %1)\t%2 (%3) (%4) (%5)%6")
                     .arg(i++)
                     .arg(s.name)
                     .arg(s.format)
                     .arg(s.lang)
                     .arg(s.engine)
                     .arg(resolution));
      }

      while (!ok) {
        std::cout << tr("   Select subtitles to download: ").toStdString();
        char line[8];
        std::cin.getline(line, 8);

        selIdx = QString(line).toInt(&ok);

        if (!ok) {
          printCli(tr("   You must enter a number!"));
          std::cin.clear();
        } else if ((selIdx > list.size()) || (selIdx < 0)) {
          ok = false;
          printCli(tr("   Enter a number from the list!"));
        }
      }

      --selIdx;
    } else {
      selIdx = napi.bestIdx();
    }

    if (selIdx == -1) continue;

    printCli(tr("   Downloading subtitles..."));
    if (!napi.download(selIdx)) {
      printCli(tr("   Unable to download subtitles!"));
      return EC_COULD_NOT_DOWNLOAD;
    }

    printCli(tr("   Unpacking subtitles..."));
    if (!napi.unpack(selIdx)) {
      printCli(tr("   Failed to unpack subtitles!"));
      return EC_COULD_NOT_UNARCHIVE;
    }

    if (napi.ppEnabled()) {
      printCli(tr("   Post-processing subtitles file..."));
      napi.postProcessSubtitles();
    }

    printCli(tr("   Adjusting subtitles..."));
    if (!napi.matchSubtitles()) {
      printCli(tr("   Could not adjust subtitles!"));
      return EC_COULD_NOT_MATCH;
    }

    napi.cleanup();
  }

  return EC_OK;
}

void QNapiCli::printHeader() {
  printCli(tr("QNapi %1, %2")
               .arg(LibQNapi::displayableVersion())
               .arg(LibQNapi::webpageUrl()));
  printCli(tr("Qt version: %1").arg(qVersion()));
  printCli();
}

void QNapiCli::printHelp(const QString &binaryFileName) {
  QString formats =
      LibQNapi::subtitleFormatsRegistry()->listFormatNames().join(",");

  printCli(tr("QNapi is distributed under the GNU General Public License v2."));
  printCli();
  printCli(tr("Syntax: %1 [options] [list of files]").arg(binaryFileName));
  printCli(tr("Available options:"));
  printCli(tr("   -c, --console              Download subtitles with console"));
  printCli(
      tr("   -q, --quiet                Download subtitles quietly without "
         "showing"));
  printCli(
      tr("                              any messages or windows (implies -d)"));
  printCli();
  printCli(
      tr("   -s, --show-list            Show a list of subtitles (works only "
         "with -c)"));
  printCli(
      tr("   -d, --dont-show-list       Do not show a list of subtitles (works "
         "only with -c)"));
  printCli();
  printCli(tr("   -l, --lang [code]          Preferred subtitles language"));
  printCli(tr("   -lb,--lang-backup [code]   Alternative subtitles language"));
  printCli(tr("   -f, --format [format]      Select target subtitles file "
              "format (%1)")
               .arg(formats));
  printCli(tr(
      "   -e, --extension [ext]      Select target subtitles file extension"));
  printCli();
  printCli(tr("   -o, --options              Show program options (only GUI)"));
  printCli(tr("   -h, --help                 Show help text"));
  printCli(tr(
      "   -hl,--help-languages       List of available subtitles languages"));
  printCli();
}

void QNapiCli::printHelpLanguages() {
  printCli(
      tr("List of languages recognized by QNapi, including corresponding"));
  printCli(tr("two-letter language codes:"));
  printCli();

  SubtitleLanguage L, LB;
  QStringList langs = L.listLanguages();

  foreach (QString lang, langs) {
    L.setLanguage(lang);
    printCli(QString(" %1 - %2").arg(L.toTwoLetter()).arg(lang));
  }

  L.setLanguage(config.generalConfig().language());
  LB.setLanguage(config.generalConfig().backupLanguage());

  printCli();
  printCli(tr("Current default subtitles language: %1 (%2)")
               .arg(L.toFullName())
               .arg(L.toTwoLetter()));

  if (LB.toFullName().isEmpty()) {
    printCli(tr("No alternative subtitles language has been set"));
  } else {
    printCli(tr("Current alternative subtitles language: %1 (%2)")
                 .arg(LB.toFullName())
                 .arg(LB.toTwoLetter()));
  }
}

void QNapiCli::printCli(const QString &string) {
  if (mode != CM_QUIET) std::cout << string.toStdString() << std::endl;
}

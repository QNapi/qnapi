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

#include "guimain.h"
#include <signal.h>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcess>
#include <iostream>
#include "config/qnapiconfig.h"
#include "libqnapi.h"
#include "qnapicommand.h"
#include "utils/helphelper.h"

namespace GuiMain {

int processCommand(QNapiApp &app, QVariant cliCommand,
                   const QNapiConfig &config,
                   const QList<QSharedPointer<CliArgParser>> &cliArgParsers) {
  using namespace QNapiCommand;

  if (cliCommand.canConvert<RunCLIApp>()) {
    QStringList cliArgs = cliCommand.value<RunCLIApp>().arguments;
    return GuiMain::runCLI(cliArgs);

  } else if (cliCommand.canConvert<ShowHelp>()) {
    auto helpLines = HelpHelper::formatHelpLinesText(cliArgParsers, 30, 64);

    showHelpText(helpLines.join("\n"));

    return 0;
  } else if (cliCommand.canConvert<ShowHelpLanguages>()) {
    showHelpLanguages(config);

    return 0;
  } else if (cliCommand.canConvert<ShowOptions>()) {
    app.showSettings();
    return 0;

  } else if (cliCommand.canConvert<ScanDirectory>()) {
    app.setQuitOnLastWindowClosed(true);
    app.progress()->setBatchMode(true);
    app.showScanDialog(config, cliCommand.value<ScanDirectory>().directory);
    return app.exec();

  } else if (cliCommand.canConvert<DownloadSubtitles>()) {
    QStringList movieFilePaths =
        cliCommand.value<DownloadSubtitles>().movieFilePaths;

    if (!app.isInstanceAllowed()) {
      foreach (QString movieFilePath, movieFilePaths) {
        QFileInfo fi(movieFilePath);
        app.sendRequest(fi.absoluteFilePath());
      }
      return 0;
    }

    app.setQuitOnLastWindowClosed(true);
    app.progress()->setBatchMode(true);
    app.progress()->enqueueFiles(movieFilePaths);
    app.progress()->download(config);
    return app.exec();
  }

  return 1;
}

void showHelpText(const QString &helpText) {
  QMessageBox helpDialog(QMessageBox::Information, "QNapi", helpText,
                         QMessageBox::Close);

  QSpacerItem *horizontalSpacer =
      new QSpacerItem(720, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  QGridLayout *layout = (QGridLayout *)helpDialog.layout();
  layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1,
                  layout->columnCount());

  helpDialog.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  helpDialog.exec();
}

void showHelpLanguages(const QNapiConfig &config) {
  QStringList helpLanguagesLines;
  helpLanguagesLines << tr(
      "List of languages recognized by QNapi, including corresponding");
  helpLanguagesLines << tr("two-letter language codes:");
  helpLanguagesLines << "";

  SubtitleLanguage L, LB;
  QStringList langs = L.listLanguages();

  foreach (QString lang, langs) {
    L.setLanguage(lang);
    helpLanguagesLines << QString(" %1 - %2").arg(L.toTwoLetter()).arg(lang);
  }

  L.setLanguage(config.generalConfig().language());
  LB.setLanguage(config.generalConfig().backupLanguage());

  helpLanguagesLines << "";
  helpLanguagesLines << tr("Current default subtitles language: %1 (%2)")
                            .arg(L.toFullName())
                            .arg(L.toTwoLetter());

  if (LB.toFullName().isEmpty()) {
    helpLanguagesLines << tr("No alternative subtitles language has been set");
  } else {
    helpLanguagesLines << tr("Current alternative subtitles language: %1 (%2)")
                              .arg(LB.toFullName())
                              .arg(LB.toTwoLetter());
  }

  showHelpText(helpLanguagesLines.join("\n"));
}

void showArgParserError(const QString &errorMessage) {
  QMessageBox::critical(0, tr("QNapi - command line argument parsing error"),
                        errorMessage, QMessageBox::Close);
}

static QString cliExecutableName =
#ifdef Q_OS_WIN
    "qnapic.exe";
#else
    "qnapic";
#endif

Maybe<QString> cliExecutablePath() {
  QStringList searchPaths = {LibQNapi::appExecutableDir};

#ifndef Q_OS_WIN
  searchPaths << QProcess::systemEnvironment()
                     .filter(QRegExp("^PATH=(.*)$"))
                     .value(0)
                     .mid(5)
                     .split(":")
              << "/bin"
              << "/usr/bin"
              << "/usr/local/bin";
  searchPaths.removeAll("");
#endif

  foreach (QString path, searchPaths) {
    QString candidate = path + QDir::separator() + cliExecutableName;
    if (QFileInfo(candidate).isExecutable()) {
      return just(candidate);
    }
  }

  return nothing();
}

bool isCLIAvailable() { return cliExecutablePath(); }

int runCLI(const QStringList &args) {
  auto maybeCliExecutablePath = cliExecutablePath();
  if (!maybeCliExecutablePath) {
    QMessageBox::critical(0, tr("QNapi - CLI executable not found"),
                          tr("You have requested to run QNapi in command-line "
                             "mode, but unfortunately command-line binary '%1' "
                             "could not be found.")
                              .arg(cliExecutableName),
                          QMessageBox::Close);
    return 1;
  } else {
    return QProcess::execute(maybeCliExecutablePath.value(), args);
  }
}

void installTranslation(QCoreApplication &app, QTranslator *qtTranslator,
                        QTranslator *translator, const QNapiConfig &config) {
  QString uiLanguage = LibQNapi::uiLanguage(config.generalConfig());

  QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

  qtTranslator->load("qt_" + uiLanguage, resourceDir);
  app.installTranslator(qtTranslator);

  translator->load("qnapi_" + uiLanguage, ":/translations");
  app.installTranslator(translator);
}

void regSignal() {
#ifdef Q_OS_WIN
  signal(SIGTERM, sigHandler);
  signal(SIGINT, sigHandler);
#else
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = sigHandler;
  sigaction(SIGTERM, &sa, 0);
  sigaction(SIGINT, &sa, 0);
#endif
}

void sigHandler(int sig) {
  Q_UNUSED(sig);

  std::cout << std::endl
            << QObject::tr("QNapi: deleting temporary files...").toStdString()
            << std::endl;

  const QNapiConfig config = LibQNapi::loadConfig();
  QString tmpPath = config.generalConfig().tmpPath();
  QDir tmpDir(tmpPath);

  QStringList filters;
  filters << "QNapi-*-rc";
  filters << "QNapi.*.tmp";

  QFileInfoList files = tmpDir.entryInfoList(filters);

  foreach (QFileInfo file, files) { QFile::remove(file.filePath()); }

  std::cout << QObject::tr("QNapi: finished.").toStdString() << std::endl;

  exit(666);
}

}  // namespace GuiMain

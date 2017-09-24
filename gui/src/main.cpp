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

#include <QMessageBox>
#include <QStringList>
#include <QTranslator>
#include <QVariant>

#include "Either.h"
#include "Maybe.h"
#include "guimain.h"
#include "libqnapi.h"
#include "parser/backuplangargparser.h"
#include "parser/cliargparser.h"
#include "parser/cliargparsersexecutor.h"
#include "parser/dontshowlistargparser.h"
#include "parser/downloadsubtitlesargparser.h"
#include "parser/extensionargparser.h"
#include "parser/formatargparser.h"
#include "parser/langargparser.h"
#include "parser/quietbatchargparser.h"
#include "parser/runcliappargparser.h"
#include "parser/scandirectoryargparser.h"
#include "parser/showhelpargparser.h"
#include "parser/showhelplanguagesargparser.h"
#include "parser/showlistargparser.h"
#include "parser/showoptionsargparser.h"
#include "qnapiapp.h"

int main(int argc, char **argv) {
  LibQNapi::init(argv[0]);

  const QNapiConfig config = LibQNapi::loadConfig();

  QNapiApp app(argc, argv, "QNapi");

  GuiMain::regSignal();

  QTranslator qtTranslator, qnapiTranslator;
  GuiMain::installTranslation(app, &qtTranslator, &qnapiTranslator, config);

  QList<QSharedPointer<CliArgParser>> cliArgParsers = {
      QSharedPointer<CliArgParser>(new ShowHelpArgParser()),
      QSharedPointer<CliArgParser>(new ShowHelpLanguagesArgParser()),
      QSharedPointer<CliArgParser>(new RunCLIAppArgParser()),
      QSharedPointer<CliArgParser>(new QuietBatchArgParser()),
      QSharedPointer<CliArgParser>(new ShowListArgParser()),
      QSharedPointer<CliArgParser>(new DontShowListArgParser()),
      QSharedPointer<CliArgParser>(new LangArgParser()),
      QSharedPointer<CliArgParser>(new BackupLangArgParser()),
      QSharedPointer<CliArgParser>(new FormatArgParser()),
      QSharedPointer<CliArgParser>(new ExtensionArgParser()),
      QSharedPointer<CliArgParser>(new ShowOptionsArgParser()),
      QSharedPointer<CliArgParser>(new ScanDirectoryArgParser()),
      QSharedPointer<CliArgParser>(new DownloadSubtitlesArgParser())};

  QStringList tailArgs = app.arguments().mid(1, app.arguments().size() - 1);

  auto parseResult =
      CliArgParsersExecutor::executeParsers(cliArgParsers, tailArgs, config);

  if (parseResult.is<Maybe<CliArgParser::ParsedCommand>>()) {
    auto parsedMaybeCommand =
        parseResult.as<Maybe<CliArgParser::ParsedCommand>>();

    if (parsedMaybeCommand) {
      auto parsedCommand = parsedMaybeCommand.value();
      return GuiMain::processCommand(app, parsedCommand.command,
                                     parsedCommand.refinedConfig,
                                     cliArgParsers);
    } else {
      if (app.isInstanceAllowed()) {
        if (config.firstrun()) {
          if (QMessageBox::question(
                  0, QObject::tr("First Run"),
                  QObject::tr("This is the first launch of the "
                              "program QNapi. Do you want to "
                              "configure it now?"),
                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            app.showSettings();
          }
        }

        if (!QSystemTrayIcon::isSystemTrayAvailable()) {
          app.setQuitOnLastWindowClosed(true);
          if (!app.showOpenDialog(config)) return 1;
        } else {
          app.setQuitOnLastWindowClosed(false);
          app.createTrayIcon();
        }
        app.exec();
      }
      return 0;
    }

  } else {
    GuiMain::showArgParserError(parseResult.as<QString>());
    return 1;
  }
}

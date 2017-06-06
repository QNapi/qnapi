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
#include "config/qnapiconfig.h"
#include "libqnapi.h"

#include <signal.h>
#include <QDir>
#include <iostream>

namespace GuiMain {

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

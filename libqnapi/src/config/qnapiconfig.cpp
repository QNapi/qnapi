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

#include "config/qnapiconfig.h"
#include <QTextStream>

QString QNapiConfig::toString() const {
  QString enabledEnginesStr, enginesCfgStr;

  QTextStream ees(&enabledEnginesStr);
  typedef QPair<QString, bool> EngineEnableCfg;
  foreach (EngineEnableCfg engineCfg, enabledEngines()) {
    QString engineName = engineCfg.first;
    bool engineEnabled = engineCfg.second;
    QString engineEnabledStr = engineEnabled ? "enabled" : "disabled";
    ees << " " << engineName << ": " << engineEnabledStr << endl;
  }

  QTextStream es(&enginesCfgStr);
  foreach (QString engineName, enginesConfig().keys()) {
    EngineConfig cfg = *enginesConfig().find(engineName);
    es << " " << engineName << ": " << endl
       << "  nick: " << cfg.nick() << endl
       << "  password: " << cfg.password() << endl;
  }

  QString s;
  QTextStream(&s) << "Version: " << version() << endl
                  << "First run? " << (firstrun() ? "yes" : "no") << endl
                  << endl
                  << "General config:" << endl
                  << generalConfig().toString() << endl
                  << "Enabled Engines:" << endl
                  << enabledEnginesStr << endl
                  << "Engines config:" << endl
                  << enginesCfgStr << endl
                  << "Post-processing config:" << endl
                  << postProcessingConfig().toString() << endl
                  << "Scan config:" << endl
                  << scanConfig().toString() << endl
                  << "Last open-dialog dir:" << endl
                  << lastOpenedDir() << endl;
  return s;
}

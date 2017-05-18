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

#include "config/generalconfig.h"
#include <QTextStream>

QString GeneralConfig::toString() const {
  QString s;
  QTextStream(&s) << "uiLanguage: " << uiLanguage() << endl
                  << "p7zipPath: " << p7zipPath() << endl
                  << "tmpPath: " << tmpPath() << endl
                  << "language: " << language() << endl
                  << "backupLanguage: " << backupLanguage() << endl
                  << "noBackup: " << noBackup() << endl
                  << "quietBatch: " << quietBatch() << endl
                  << "searchPolicy: " << searchPolicy() << endl
                  << "downloadPolicy: " << downloadPolicy() << endl
                  << "changePermissions: " << changePermissionsEnabled() << endl
                  << "changePermissionsTo: " << changePermissionsTo() << endl;
  return s;
}

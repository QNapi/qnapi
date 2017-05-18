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

#include "config/postprocessingconfig.h"
#include <QTextStream>

QString PostProcessingConfig::toString() const {
  QString s;
  QTextStream(&s) << "enabled: " << enabled() << endl
                  << "encodingChangeMethod: " << encodingChangeMethod() << endl
                  << "encodingFrom: " << encodingFrom() << endl
                  << "encodingAutoDetectFrom: " << encodingAutoDetectFrom()
                  << endl
                  << "encodingTo: " << encodingTo() << endl
                  << "showAllEncodings: " << showAllEncodings() << endl
                  << "subFormat: " << subFormat() << endl
                  << "subExtension: " << subExtension() << endl
                  << "skipConvertAds: " << skipConvertAds() << endl
                  << "removeWordsEnabled: " << removeWordsEnabled() << endl
                  << "removeWords: " << removeWords().join("; ") << endl;
  return s;
}

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

#include "pathutils.h"

#include <QDir>
#include <QTextStream>

QString ChangeFilePath::apply(const QString &path) const {
  // search backward and only until directory separator,
  // both 'from' and 'to' are inclusive
  auto findDot = [&path] (int from, int to = 0) {
    for (; from >= to; from--) {
      if (path[from] == '.') return from;
      if (path[from] == QDir::separator()) break;
    }
    return -1;
  };

  // position of the dot that precedes file extension
  int extDotPos = findDot(path.length() - 1);
  // accounts for file extension including the dot
  int extLength = extDotPos < 0 ? 0 : path.length() - extDotPos;

  // accounts for existing language code including the preceding dot
  int langCodeLength = 0;
  if (extDotPos >= 0 &&
      // no need to bother if we don't need it
      (!addToBaseNameStr.isEmpty() || doRemoveLanguageCode)) {
    int dotPos = findDot(extDotPos - 1, qMax(0, extDotPos - 4));
    if (dotPos >= 0) {
      SubtitleLanguage lang(path.mid(dotPos + 1, extDotPos - dotPos - 1));
      if (lang.isValid()) langCodeLength = extDotPos - dotPos;
    }
  }

  QString ret = path.left(path.length() - extLength - langCodeLength);

  ret += addToBaseNameStr;

  if (!doRemoveLanguageCode && langCodeLength > 0) {
    ret += path.midRef(
        path.length() - extLength - langCodeLength, langCodeLength);
  }

  if (addLanguageCodeLang.isValid() && addLanguageCodeType != LCT_NONE) {
    ret += '.';
    ret += addLanguageCodeLang.toString(addLanguageCodeType);
  }

  if (!doRemoveExtension) ret += path.rightRef(extLength);

  if (!addExtensionStr.isEmpty()) {
    ret += '.';
    ret += addExtensionStr;
  }

  return ret;
}

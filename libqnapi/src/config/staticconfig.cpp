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

#include "staticconfig.h"

StaticConfig::StaticConfig() {}

QStringList StaticConfig::movieExtensions() const {
  static QStringList exts = {"avi",  "asf", "divx", "mkv", "mov",  "mp4",
                             "mpeg", "mpg", "ogm",  "rm",  "rmvb", "wmv"};
  return exts;
}

QString StaticConfig::movieExtensionsFilter() const {
  static QString movieExtFilter = makeFilter(movieExtensions());
  return movieExtFilter;
}

QStringList StaticConfig::subtitleExtensions() const {
  static QStringList exts = {"srt", "sub", "txt"};
  return exts;
}

QString StaticConfig::subtitleExtensionsFilter() const {
  static QString subtitleExtFilter = makeFilter(subtitleExtensions());
  return subtitleExtFilter;
}

QString StaticConfig::makeFilter(const QStringList& extensions) const {
  QStringList patterns;
  foreach (QString extension, extensions) { patterns << "*." + extension; }
  return patterns.join(" ");
}

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

#ifndef ENCODINGUTILS_H
#define ENCODINGUTILS_H

#include <QByteArray>
#include <QString>
#include <QStringList>

class EncodingUtils {
 public:
  EncodingUtils();

  QString replaceDiacriticsWithASCII(const QString& str) const;
  QString detectBufferEncoding(const QByteArray& buffer) const;
  QString detectFileEncoding(const QString& filename) const;

 private:
  QString diacritics;
  QStringList replacements;
  QStringList codecs;
};

#endif  // ENCODINGUTILS_H

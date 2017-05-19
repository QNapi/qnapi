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

#include "encodingutils.h"

#include <QFile>
#include <QTextCodec>
#include <QTextStream>

EncodingUtils::EncodingUtils() {
  diacritics = QString::fromUtf8(
      "ąćęłńśżźĄĆĘŁŃŚŻŹŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïð"
      "ñòóôõöøùúûüýÿ");
  replacements << "a"
               << "c"
               << "e"
               << "l"
               << "n"
               << "s"
               << "z"
               << "z"
               << "A"
               << "C"
               << "E"
               << "L"
               << "N"
               << "S"
               << "Z"
               << "Z"
               << "S"
               << "OE"
               << "Z"
               << "s"
               << "oe"
               << "z"
               << "Y"
               << "Y"
               << "u"
               << "A"
               << "A"
               << "A"
               << "A"
               << "A"
               << "A"
               << "AE"
               << "C"
               << "E"
               << "E"
               << "E"
               << "E"
               << "I"
               << "I"
               << "I"
               << "I"
               << "D"
               << "N"
               << "O"
               << "O"
               << "O"
               << "O"
               << "O"
               << "O"
               << "U"
               << "U"
               << "U"
               << "U"
               << "Y"
               << "s"
               << "a"
               << "a"
               << "a"
               << "a"
               << "a"
               << "a"
               << "ae"
               << "c"
               << "e"
               << "e"
               << "e"
               << "e"
               << "i"
               << "i"
               << "i"
               << "i"
               << "o"
               << "n"
               << "o"
               << "o"
               << "o"
               << "o"
               << "o"
               << "o"
               << "u"
               << "u"
               << "u"
               << "u"
               << "y"
               << "y";

  codecs << "windows-1257"
         << "ISO-8859-13"
         << "ISO-8859-16"
         << "ISO-8859-2"
         << "windows-1250"
         << "UTF-8";
}

QString EncodingUtils::replaceDiacriticsWithASCII(const QString &str) const {
  QString output;
  for (int i = 0; i < str.length(); i++) {
    QChar c = str[i];
    int dIndex = diacritics.indexOf(c);
    if (dIndex < 0) {
      output.append(c);
    } else {
      QString replacement = replacements[dIndex];
      output.append(replacement);
    }
  }
  return output;
}

QString EncodingUtils::detectBufferEncoding(const QByteArray &buffer) const {
  QTextCodec::ConverterState state;
  QTextCodec::codecForName("UTF-8")->toUnicode(buffer.constData(),
                                               buffer.length(), &state);

  if (state.invalidChars == 0) return "UTF-8";

  int bestMatch = 0;
  QString from;

  foreach (QString codec, codecs) {
    QTextCodec *tc = QTextCodec::codecForName(qPrintable(codec));

    const QString text = tc->toUnicode(buffer.constData(), buffer.size());

    QStringList chars = QString::fromUtf8("ą/ś/ż/ć/ń/ł/ó/ę").split("/");

    int found = 0;

    foreach (QString c, chars) {
      if (text.contains(c, Qt::CaseInsensitive)) {
        ++found;
      }
    }

    if (found >= bestMatch) {
      bestMatch = found;
      from = codec;
    }
  }

  return from;
}

QString EncodingUtils::detectFileEncoding(const QString &filename) const {
  QFile f(filename);
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString enc = detectBufferEncoding(f.readAll());
    f.close();
    return enc;
  }
  return "";
}

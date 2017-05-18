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

#include "tmplayer.h"

bool TMPlayerSubtitleFormat::detect(const QStringList &lines) const {
  foreach (QString line, lines) {
    if (!line.trimmed().isEmpty()) {
      QRegExp r("^(0\\d|\\d\\d):(0\\d|\\d\\d):(0\\d|\\d\\d):(.*)");
      r.setPatternSyntax(QRegExp::RegExp2);
      return r.exactMatch(line);
    }
  }

  return false;
}

SubFile TMPlayerSubtitleFormat::decode(const QStringList &lines) const {
  SubFile sf;

  foreach (QString line, lines) {
    if (!line.trimmed().isEmpty()) {
      QRegExp r("^(0\\d|\\d\\d):(0\\d|\\d\\d):(0\\d|\\d\\d):(.*)");
      r.setPatternSyntax(QRegExp::RegExp2);
      if (r.exactMatch(line)) {
        int h = r.cap(1).toInt();
        int m = r.cap(2).toInt();
        int s = r.cap(3).toInt();
        QString tokenStream = r.cap(4);

        SubEntry se;
        se.frameStart = 1000L * (3600L * h + 60L * m + s);
        se.tokens = decodeTokenStream(tokenStream);

        sf.entries.push_back(se);
      }
    }
  }

  for (int i = 0; i < sf.entries.size(); ++i) {
    long plus5s = sf.entries[i].frameStart + 5000L;
    sf.entries[i].frameStop = plus5s;
    if (i < sf.entries.size() - 1) {
      sf.entries[i].frameStop =
          std::min(plus5s, sf.entries[i + 1].frameStart - 1);
    }
  }

  return sf;
}

QStringList TMPlayerSubtitleFormat::encode(const SubFile &subFile) const {
  QStringList lines;
  foreach (const SubEntry &entry, subFile.entries) {
    lines.append(encodeEntry(entry));
  }
  return lines;
}

QString TMPlayerSubtitleFormat::encodeEntry(const SubEntry &entry) const {
  QString tokensBuff;
  foreach (const SubToken &token, entry.tokens) {
    tokensBuff += encodeToken(token);
  }
  long ss = entry.frameStart / 1000;
  int h = ss / 3600;
  int m = (ss - h * 3600) / 60;
  int s = ss - h * 3600 - m * 60;
  return QString("%1:%2:%3: %4")
      .arg(h, 2, 10, QChar('0'))
      .arg(m, 2, 10, QChar('0'))
      .arg(s, 2, 10, QChar('0'))
      .arg(tokensBuff);
}

QString TMPlayerSubtitleFormat::encodeToken(const SubToken &entry) const {
  switch (entry.type) {
    case STT_WS:
      return " ";
    case STT_WORD:
      return entry.payload;
    case STT_NEWLINE:
      return "|";
    case STT_BOLD:
      return "{y:b}";
    case STT_ITALIC:
      return "{y:i}";
    case STT_UNDERLINE:
      return "{y:u}";
    case STT_FONTCOLOR:
      return QString("{c:$%1}").arg(entry.payload);
    default:
      return "";
  }
}

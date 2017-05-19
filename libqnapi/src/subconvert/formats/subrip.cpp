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

#include "subrip.h"

bool SubRipSubtitleFormat::detect(const QStringList &lines) const {
  QStringList lines2 = lines;
  while (!lines2.isEmpty() && lines2.first().trimmed().isEmpty()) {
    lines2.pop_front();
  }

  QString firstEntryBuff;

  foreach (QString line, lines2) {
    if (line.isEmpty()) break;
    firstEntryBuff += line + "\n";
  }

  QRegExp r(
      "^(\\d+)(\\n|\\r\\n|\\s+)(\\d{2}):(\\d{2}):(\\d{2})\\,(\\d{3})\\s+\\-\\->"
      "\\s+(\\d{2}):(\\d{2}):(\\d{2})\\,(\\d{3})(.*)");

  return r.exactMatch(firstEntryBuff);
}

SubFile SubRipSubtitleFormat::decode(const QStringList &lines) const {
  SubFile sf;

  QRegExp r1(
      "^(\\d+)\\s+(\\d{2}):(\\d{2}):(\\d{2})\\,(\\d{3})\\s+\\-\\->\\s+(\\d{2}):"
      "(\\d{2}):(\\d{2})\\,(\\d{3})(.*)");
  QRegExp r2(
      "^(\\d{2}):(\\d{2}):(\\d{2})\\,(\\d{3})\\s+\\-\\->\\s+(\\d{2}):(\\d{2}):("
      "\\d{2})\\,(\\d{3})(.*)");
  QRegExp rNumLine("^\\d+");

  QString tokensBuff = "", numsBuff;
  SrtTimestamps tss;

  foreach (QString line, lines) {
    if (r1.exactMatch(line)) {
      if (!tokensBuff.isEmpty()) addEntry(sf.entries, tokensBuff, tss);
      tss.h1 = r1.cap(2).toInt();
      tss.m1 = r1.cap(3).toInt();
      tss.s1 = r1.cap(4).toInt();
      tss.ms1 = r1.cap(5).toInt();
      tss.h2 = r1.cap(6).toInt();
      tss.m2 = r1.cap(7).toInt();
      tss.s2 = r1.cap(8).toInt();
      tss.ms2 = r1.cap(9).toInt();
      numsBuff.clear();
    } else if (r2.exactMatch(line)) {
      if (!tokensBuff.isEmpty()) addEntry(sf.entries, tokensBuff, tss);
      tss.h1 = r2.cap(1).toInt();
      tss.m1 = r2.cap(2).toInt();
      tss.s1 = r2.cap(3).toInt();
      tss.ms1 = r2.cap(4).toInt();
      tss.h2 = r2.cap(5).toInt();
      tss.m2 = r2.cap(6).toInt();
      tss.s2 = r2.cap(7).toInt();
      tss.ms2 = r2.cap(8).toInt();
      numsBuff.clear();
    } else if (rNumLine.exactMatch(line)) {
      numsBuff += line + "\n";
    } else if (!line.trimmed().isEmpty()) {
      if (!numsBuff.isEmpty()) {
        tokensBuff += numsBuff;
        numsBuff.clear();
      }
      tokensBuff += line + "\n";
    }
  }

  if (!tokensBuff.isEmpty()) addEntry(sf.entries, tokensBuff, tss);

  return sf;
}

QStringList SubRipSubtitleFormat::encode(const SubFile &subFile) const {
  QStringList lines;
  int i = 1;
  foreach (const SubEntry &entry, subFile.entries) {
    lines << encodeEntry(entry, i++);
    lines << "";
  }
  return lines;
}

void SubRipSubtitleFormat::addEntry(QVector<SubEntry> &entries,
                                    QString &tokensBuff,
                                    SrtTimestamps &timestamps) const {
  SubEntry se;
  se.frameStart = 3600000L * timestamps.h1 + 60000L * timestamps.m1 +
                  1000L * timestamps.s1 + timestamps.ms1;
  se.frameStop = 3600000L * timestamps.h2 + 60000L * timestamps.m2 +
                 1000L * timestamps.s2 + timestamps.ms2;
  se.tokens = decodeTokenStream(tokensBuff);

  while (!se.tokens.isEmpty() && se.tokens.back().type == STT_NEWLINE)
    se.tokens.pop_back();

  entries.push_back(se);
  tokensBuff.clear();
}

QStringList SubRipSubtitleFormat::encodeEntry(const SubEntry &entry,
                                              int i) const {
  QString tokensBuff;
  foreach (const SubToken &token, entry.tokens) {
    tokensBuff += encodeToken(token);
  }

  int h1 = entry.frameStart / 3600000;
  int m1 = (entry.frameStart - h1 * 3600000) / 60000;
  int s1 = (entry.frameStart - h1 * 3600000 - m1 * 60000) / 1000;
  int ms1 = entry.frameStart - h1 * 3600000 - m1 * 60000 - s1 * 1000;

  int h2 = entry.frameStop / 3600000;
  int m2 = (entry.frameStop - h2 * 3600000) / 60000;
  int s2 = (entry.frameStop - h2 * 3600000 - m2 * 60000) / 1000;
  int ms2 = entry.frameStop - h2 * 3600000 - m2 * 60000 - s2 * 1000;

  QStringList out;
  out << QString::number(i)
      << QString("%1:%2:%3,%4 --> %5:%6:%7,%8")
             .arg(h1, 2, 10, QChar('0'))
             .arg(m1, 2, 10, QChar('0'))
             .arg(s1, 2, 10, QChar('0'))
             .arg(ms1, 3, 10, QChar('0'))
             .arg(h2, 2, 10, QChar('0'))
             .arg(m2, 2, 10, QChar('0'))
             .arg(s2, 2, 10, QChar('0'))
             .arg(ms2, 3, 10, QChar('0'))
      << tokensBuff.split("\n");
  return out;
}

QString SubRipSubtitleFormat::encodeToken(const SubToken &entry) const {
  switch (entry.type) {
    case STT_WS:
      return " ";
    case STT_WORD:
      return entry.payload;
    case STT_NEWLINE:
      return "\n";
    case STT_BOLD:
      return "<b>";
    case STT_BOLD_END:
      return "</b>";
    case STT_ITALIC:
      return "<i>";
    case STT_ITALIC_END:
      return "</i>";
    case STT_UNDERLINE:
      return "<u>";
    case STT_UNDERLINE_END:
      return "</u>";
    case STT_FONTCOLOR:
      return QString("<font color=\"#%1\">").arg(entry.payload);
    case STT_FONTCOLOR_END:
      return "</font>";
    default:
      return "";
  }
}

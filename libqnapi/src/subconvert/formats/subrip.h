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

#ifndef SUBRIPSUBTITLEFORMAT_H
#define SUBRIPSUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class SubRipSubtitleFormat : public SubtitleFormat {
 public:
  virtual ~SubRipSubtitleFormat() {}

  bool isTimeBased() const { return true; }
  QString formatName() const { return "SRT"; }
  QString defaultExtension() const { return "srt"; }

  bool detect(const QStringList &lines) const;
  SubFile decode(const QStringList &lines) const;
  QStringList encode(const SubFile &subFile) const;

 private:
  struct SrtTimestamps {
    int h1;
    int m1;
    int s1;
    int ms1;
    int h2;
    int m2;
    int s2;
    int ms2;
  };

  void addEntry(QVector<SubEntry> &entries, QString &tokensBuff,
                SrtTimestamps &timestamps) const;
  QStringList encodeEntry(const SubEntry &entry, int i) const;
  QString encodeToken(const SubToken &entry) const;
};

#endif  // SUBRIPSUBTITLEFORMAT_H

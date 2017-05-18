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

#ifndef SUBFILE_H
#define SUBFILE_H

#include <QStringList>
#include <QVector>

enum SubTokenType {
  STT_WS = 0,
  STT_WORD = 1,
  STT_NEWLINE = 2,
  STT_BOLD = 3,
  STT_BOLD_END = 4,
  STT_ITALIC = 5,
  STT_ITALIC_END = 6,
  STT_UNDERLINE = 7,
  STT_UNDERLINE_END = 8,
  STT_FONTCOLOR = 9,
  STT_FONTCOLOR_END = 10
};

struct SubToken {
  SubTokenType type;
  QString payload;

  bool operator==(const SubToken& other) const {
    return type == other.type && payload == other.payload;
  }
};

struct SubEntry {
  long frameStart, frameStop;
  // if time-based, they represent frames as milliseconds
  // if frame-based, they represent exact frame numbers

  QVector<SubToken> tokens;
};

struct SubFile {
  QVector<SubEntry> entries;
};

#endif  // SUBFILE_H

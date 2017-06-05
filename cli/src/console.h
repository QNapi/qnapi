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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QString>

class Console {
  bool isQuiet;

 public:
  Console(bool isQuiet = false);
  void printLine(const QString& line = "") const;
  void printLineHighlihted(const QString& line) const;
  void printLineOrdinary(const QString& line) const;
  void printLineError(const QString& line) const;
};

#endif  // CONSOLE_H

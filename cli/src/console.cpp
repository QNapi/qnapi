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

#include "console.h"

#include <iostream>

Console::Console(bool isQuiet) : isQuiet(isQuiet) {}

void Console::printLine(const QString& line) const {
  if (isQuiet) return;
  std::cout << line.toStdString() << std::endl;
}

void Console::print(const QString& line) const {
  if (isQuiet) return;
  std::cout << line.toStdString();
}

void Console::printLineHighlihted(const QString& line) const {
  printLine(" * " + line);
}

void Console::printLineOrdinary(const QString& line) const {
  printLine("   " + line);
}

void Console::printOrdinary(const QString& line) const { print("   " + line); }

void Console::printLineError(const QString& line) const {
  printLine("!! " + line);
}

void Console::printLineWarning(const QString& line) const {
  printLine(" ! " + line);
}

int Console::inputNumber(const QString& message, int min, int max) const {
  bool ok = false;
  int number = 0;

  while (!ok) {
    printOrdinary(message);
    char line[16];
    std::cin.getline(line, 16);
    number = QString(line).toInt(&ok);
    if (!ok) {
      printLineWarning(tr("You must enter a number!"));
      std::cin.clear();
    } else if (number < min || number > max) {
      ok = false;
      printLineWarning(
          tr("You must enter a number between %1 and %2").arg(min).arg(max));
    }
  }

  return number;
}

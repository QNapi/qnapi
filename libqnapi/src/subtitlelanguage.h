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

#ifndef SUBTITLELANGUAGE_H
#define SUBTITLELANGUAGE_H

#include <QObject>
#include <QStringList>

class SubtitleLanguage {
 public:
  SubtitleLanguage(QString source = "");

  void setLanguage(QString source);

  QString toTwoLetter();
  QString toTriLetter();
  QString toFullName();

  QStringList listLanguages();
  QStringList listLanguageTwoLetterCodes();

 private:
  QStringList codes2l, codes3l, names;
  int idx;

  void fill_tables();
};

Q_DECLARE_METATYPE(SubtitleLanguage)

#endif

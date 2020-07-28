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
#include <array>

/** Language code types. */
enum LangCodeType {
  LCT_NONE = 0,       ///< No language code.
  LCT_TWO_LETTER = 1, ///< Two-letter language code.
  LCT_TRI_LETTER = 2, ///< Three-letter language code.
  LCT_END,            ///< End marker.
};

class SubtitleLanguage {
 public:
  SubtitleLanguage(QString source = "");

  void setLanguage(QString source);

  QString toString(LangCodeType langCodeType = LCT_NONE) const;

  QString toTwoLetter() const { return toString(LCT_TWO_LETTER); }
  QString toTriLetter() const { return toString(LCT_TRI_LETTER); }
  QString toFullName() const { return toString(LCT_NONE); }

  QStringList listLanguages(LangCodeType langCodeType = LCT_NONE) const;
  QStringList listLanguageTwoLetterCodes() const { return listLanguages(LCT_TWO_LETTER); }

 private:
  std::array<QStringList, LCT_END> strings;
  int idx;

  void fill_tables();
};

Q_DECLARE_METATYPE(SubtitleLanguage)

#endif

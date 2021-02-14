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

#include "subtitlelanguage.h"

SubtitleLanguage::SubtitleLanguage(QString source) {
  fill_tables();

  setLanguage(source);
}

void SubtitleLanguage::setLanguage(QString source) {
  idx = -1;

  if (source.length() == 2) {
    idx = strings[LCT_TWO_LETTER].indexOf(source.toLower());
  } else if (source.length() == 3) {
    idx = strings[LCT_TRI_LETTER].indexOf(source.toLower());
  } else {
    idx = strings[LCT_NONE].indexOf(source);
  }
}

/**
 * Convert selected language to a string.
 *
 * @param langCodeType Type of language code or #LCT_NONE to get full name of
 *                     the langage.
 * @return Language string or empty string if selected language coundn't be
 *         recognized or wasn't selected at all.
 *
 * @see #SubtitleLanguage::setLanguage
 */
QString SubtitleLanguage::toString(LangCodeType langCodeType) const {
  if (idx > -1) {
    return strings.at(langCodeType).at(idx);
  }
  return "";
}

QStringList SubtitleLanguage::listLanguages(LangCodeType langCodeType) const { return strings.at(langCodeType); }

void SubtitleLanguage::fill_tables() {
  strings[LCT_TWO_LETTER] << "sq";
  strings[LCT_TRI_LETTER] << "alb";
  strings[LCT_NONE] << QObject::tr("Albanian");

  strings[LCT_TWO_LETTER] << "en";
  strings[LCT_TRI_LETTER] << "eng";
  strings[LCT_NONE] << QObject::tr("English");

  strings[LCT_TWO_LETTER] << "ar";
  strings[LCT_TRI_LETTER] << "ara";
  strings[LCT_NONE] << QObject::tr("Arabic");

  strings[LCT_TWO_LETTER] << "bg";
  strings[LCT_TRI_LETTER] << "bul";
  strings[LCT_NONE] << QObject::tr("Bulgarian");

  strings[LCT_TWO_LETTER] << "zh";
  strings[LCT_TRI_LETTER] << "chi";
  strings[LCT_NONE] << QObject::tr("Chinese");

  strings[LCT_TWO_LETTER] << "hr";
  strings[LCT_TRI_LETTER] << "hrv";
  strings[LCT_NONE] << QObject::tr("Croatian");

  strings[LCT_TWO_LETTER] << "cs";
  strings[LCT_TRI_LETTER] << "cze";
  strings[LCT_NONE] << QObject::tr("Czech");

  strings[LCT_TWO_LETTER] << "da";
  strings[LCT_TRI_LETTER] << "dan";
  strings[LCT_NONE] << QObject::tr("Danish");

  strings[LCT_TWO_LETTER] << "et";
  strings[LCT_TRI_LETTER] << "est";
  strings[LCT_NONE] << QObject::tr("Estonian");

  strings[LCT_TWO_LETTER] << "fi";
  strings[LCT_TRI_LETTER] << "fin";
  strings[LCT_NONE] << QObject::tr("Finnish");

  strings[LCT_TWO_LETTER] << "fr";
  strings[LCT_TRI_LETTER] << "fre";
  strings[LCT_NONE] << QObject::tr("French");

  strings[LCT_TWO_LETTER] << "gl";
  strings[LCT_TRI_LETTER] << "glg";
  strings[LCT_NONE] << QObject::tr("Galician");

  strings[LCT_TWO_LETTER] << "el";
  strings[LCT_TRI_LETTER] << "ell";
  strings[LCT_NONE] << QObject::tr("Greek");

  strings[LCT_TWO_LETTER] << "he";
  strings[LCT_TRI_LETTER] << "heb";
  strings[LCT_NONE] << QObject::tr("Hebrew");

  strings[LCT_TWO_LETTER] << "es";
  strings[LCT_TRI_LETTER] << "spa";
  strings[LCT_NONE] << QObject::tr("Spanish");

  strings[LCT_TWO_LETTER] << "nl";
  strings[LCT_TRI_LETTER] << "dut";
  strings[LCT_NONE] << QObject::tr("Dutch");

  strings[LCT_TWO_LETTER] << "id";
  strings[LCT_TRI_LETTER] << "ind";
  strings[LCT_NONE] << QObject::tr("Indonesian");

  strings[LCT_TWO_LETTER] << "ja";
  strings[LCT_TRI_LETTER] << "jpn";
  strings[LCT_NONE] << QObject::tr("Japanese");

  strings[LCT_TWO_LETTER] << "ko";
  strings[LCT_TRI_LETTER] << "kor";
  strings[LCT_NONE] << QObject::tr("Korean");

  strings[LCT_TWO_LETTER] << "mk";
  strings[LCT_TRI_LETTER] << "mac";
  strings[LCT_NONE] << QObject::tr("Macedonian");

  strings[LCT_TWO_LETTER] << "de";
  strings[LCT_TRI_LETTER] << "ger";
  strings[LCT_NONE] << QObject::tr("German");

  strings[LCT_TWO_LETTER] << "no";
  strings[LCT_TRI_LETTER] << "nor";
  strings[LCT_NONE] << QObject::tr("Norwegian");

  strings[LCT_TWO_LETTER] << "oc";
  strings[LCT_TRI_LETTER] << "oci";
  strings[LCT_NONE] << QObject::tr("Occitan");

  strings[LCT_TWO_LETTER] << "fa";
  strings[LCT_TRI_LETTER] << "per";
  strings[LCT_NONE] << QObject::tr("Persian (farsi)");

  strings[LCT_TWO_LETTER] << "pl";
  strings[LCT_TRI_LETTER] << "pol";
  strings[LCT_NONE] << QObject::tr("Polish");

  strings[LCT_TWO_LETTER] << "pt";
  strings[LCT_TRI_LETTER] << "por";
  strings[LCT_NONE] << QObject::tr("Portuguese");

  strings[LCT_TWO_LETTER] << "pb";
  strings[LCT_TRI_LETTER] << "pob";
  strings[LCT_NONE] << QObject::tr("Portuguese-BR");

  strings[LCT_TWO_LETTER] << "ru";
  strings[LCT_TRI_LETTER] << "rus";
  strings[LCT_NONE] << QObject::tr("Russian");

  strings[LCT_TWO_LETTER] << "ro";
  strings[LCT_TRI_LETTER] << "rum";
  strings[LCT_NONE] << QObject::tr("Romanian");

  strings[LCT_TWO_LETTER] << "sr";
  strings[LCT_TRI_LETTER] << "scc";
  strings[LCT_NONE] << QObject::tr("Serbian");

  strings[LCT_TWO_LETTER] << "sl";
  strings[LCT_TRI_LETTER] << "slv";
  strings[LCT_NONE] << QObject::tr("Slovenian");

  strings[LCT_TWO_LETTER] << "sv";
  strings[LCT_TRI_LETTER] << "swe";
  strings[LCT_NONE] << QObject::tr("Swedish");

  strings[LCT_TWO_LETTER] << "sk";
  strings[LCT_TRI_LETTER] << "slo";
  strings[LCT_NONE] << QObject::tr("Slovak");

  strings[LCT_TWO_LETTER] << "tr";
  strings[LCT_TRI_LETTER] << "tur";
  strings[LCT_NONE] << QObject::tr("Turkish");

  strings[LCT_TWO_LETTER] << "vi";
  strings[LCT_TRI_LETTER] << "vie";
  strings[LCT_NONE] << QObject::tr("Vietnamese");

  strings[LCT_TWO_LETTER] << "hu";
  strings[LCT_TRI_LETTER] << "hun";
  strings[LCT_NONE] << QObject::tr("Hungarian");

  strings[LCT_TWO_LETTER] << "it";
  strings[LCT_TRI_LETTER] << "ita";
  strings[LCT_NONE] << QObject::tr("Italian");
}

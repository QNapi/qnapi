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
    idx = codes2l.indexOf(source.toLower());
  } else if (source.length() == 3) {
    idx = codes3l.indexOf(source.toLower());
  } else {
    idx = names.indexOf(source);
  }
}

QString SubtitleLanguage::toTwoLetter() {
  if (idx > -1) {
    return codes2l.at(idx);
  }
  return "";
}

QString SubtitleLanguage::toTriLetter() {
  if (idx > -1) {
    return codes3l.at(idx);
  }
  return "";
}

QString SubtitleLanguage::toFullName() {
  if (idx > -1) {
    return names.at(idx);
  }
  return "";
}

QStringList SubtitleLanguage::listLanguages() { return names; }

QStringList SubtitleLanguage::listLanguageTwoLetterCodes() { return codes2l; }

void SubtitleLanguage::fill_tables() {
  codes2l << "sq";
  codes3l << "alb";
  names << QObject::tr("Albanian");

  codes2l << "en";
  codes3l << "eng";
  names << QObject::tr("English");

  codes2l << "ar";
  codes3l << "ara";
  names << QObject::tr("Arabic");

  codes2l << "bg";
  codes3l << "bul";
  names << QObject::tr("Bulgarian");

  codes2l << "zh";
  codes3l << "chi";
  names << QObject::tr("Chinese");

  codes2l << "hr";
  codes3l << "hrv";
  names << QObject::tr("Croatian");

  codes2l << "cs";
  codes3l << "cze";
  names << QObject::tr("Czech");

  codes2l << "da";
  codes3l << "dan";
  names << QObject::tr("Danish");

  codes2l << "et";
  codes3l << "est";
  names << QObject::tr("Estonian");

  codes2l << "fi";
  codes3l << "fin";
  names << QObject::tr("Finnish");

  codes2l << "fr";
  codes3l << "fre";
  names << QObject::tr("French");

  codes2l << "gl";
  codes3l << "glg";
  names << QObject::tr("Galician");

  codes2l << "el";
  codes3l << "ell";
  names << QObject::tr("Greek");

  codes2l << "he";
  codes3l << "heb";
  names << QObject::tr("Hebrew");

  codes2l << "es";
  codes3l << "spa";
  names << QObject::tr("Spanish");

  codes2l << "nl";
  codes3l << "dut";
  names << QObject::tr("Dutch");

  codes2l << "id";
  codes3l << "ind";
  names << QObject::tr("Indonesian");

  codes2l << "ja";
  codes3l << "jpn";
  names << QObject::tr("Japanese");

  codes2l << "ko";
  codes3l << "kor";
  names << QObject::tr("Korean");

  codes2l << "mk";
  codes3l << "mac";
  names << QObject::tr("Macedonian");

  codes2l << "de";
  codes3l << "ger";
  names << QObject::tr("German");

  codes2l << "no";
  codes3l << "nor";
  names << QObject::tr("Norwegian");

  codes2l << "oc";
  codes3l << "oci";
  names << QObject::tr("Occitan");

  codes2l << "fa";
  codes3l << "per";
  names << QObject::tr("Persian (farsi)");

  codes2l << "pl";
  codes3l << "pol";
  names << QObject::tr("Polish");

  codes2l << "pt";
  codes3l << "por";
  names << QObject::tr("Portuguese");

  codes2l << "pb";
  codes3l << "pob";
  names << QObject::tr("Portuguese-BR");

  codes2l << "ru";
  codes3l << "rus";
  names << QObject::tr("Russian");

  codes2l << "ro";
  codes3l << "rum";
  names << QObject::tr("Romanian");

  codes2l << "sr";
  codes3l << "scc";
  names << QObject::tr("Serbian");

  codes2l << "sl";
  codes3l << "slv";
  names << QObject::tr("Slovenian");

  codes2l << "sv";
  codes3l << "swe";
  names << QObject::tr("Swedish");

  codes2l << "sk";
  codes3l << "slo";
  names << QObject::tr("Slovak");

  codes2l << "tr";
  codes3l << "tur";
  names << QObject::tr("Turkish");

  codes2l << "vi";
  codes3l << "vie";
  names << QObject::tr("Vietnamese");

  codes2l << "hu";
  codes3l << "hun";
  names << QObject::tr("Hungarian");

  codes2l << "it";
  codes3l << "ita";
  names << QObject::tr("Italian");
}

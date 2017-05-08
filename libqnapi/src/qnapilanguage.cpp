/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "qnapilanguage.h"

QNapiLanguage::QNapiLanguage(QString source) : QObject(0)
{
    fill_tables();

    setLanguage(source);
}

void QNapiLanguage::setLanguage(QString source)
{
    idx = -1;

    if(source.length() == 2)
    {
        idx = codes2l.indexOf(source.toLower());
    }
    else if(source.length() == 3)
    {
        idx = codes3l.indexOf(source.toLower());
    }
    else
    {
        idx = names.indexOf(source);
    }   
}

QString QNapiLanguage::toTwoLetter()
{
    if(idx > -1)
    {
        return codes2l.at(idx);
    }
    return "";
}

QString QNapiLanguage::toTriLetter()
{
    if(idx > -1)
    {
        return codes3l.at(idx);
    }
    return "";
}

QString QNapiLanguage::toFullName()
{
    if(idx > -1)
    {
        return names.at(idx);
    }
    return "";
}
    
QStringList QNapiLanguage::listLanguages()
{
    return names;
}

void QNapiLanguage::fill_tables()
{
    codes2l << "sq";
    codes3l << "alb";
    names << tr("Albanian");

    codes2l << "en";
    codes3l << "eng";
    names << tr("English");

    codes2l << "ar";
    codes3l << "ara";
    names << tr("Arabic");

    codes2l << "bg";
    codes3l << "bul";
    names << tr("Bulgarian");

    codes2l << "zh";
    codes3l << "chi";
    names << tr("Chinese");

    codes2l << "hr";
    codes3l << "hrv";
    names << tr("Croatian");

    codes2l << "cs";
    codes3l << "cze";
    names << tr("Czech");

    codes2l << "da";
    codes3l << "dan";
    names << tr("Danish");

    codes2l << "et";
    codes3l << "est";
    names << tr("Estonian");

    codes2l << "fi";
    codes3l << "fin";
    names << tr("Finnish");

    codes2l << "fr";
    codes3l << "fre";
    names << tr("French");

    codes2l << "gl";
    codes3l << "glg";
    names << tr("Galician");

    codes2l << "el";
    codes3l << "ell";
    names << tr("Greek");

    codes2l << "he";
    codes3l << "heb";
    names << tr("Hebrew");

    codes2l << "es";
    codes3l << "spa";
    names << tr("Spanish");

    codes2l << "nl";
    codes3l << "dut";
    names << tr("Dutch");

    codes2l << "id";
    codes3l << "ind";
    names << tr("Indonesian");

    codes2l << "ja";
    codes3l << "jpn";
    names << tr("Japanese");

    codes2l << "ko";
    codes3l << "kor";
    names << tr("Korean");

    codes2l << "mk";
    codes3l << "mac";
    names << tr("Macedonian");

    codes2l << "de";
    codes3l << "ger";
    names << tr("German");

    codes2l << "no";
    codes3l << "nor";
    names << tr("Norwegian");

    codes2l << "oc";
    codes3l << "oci";
    names << tr("Occitan");

    codes2l << "fa";
    codes3l << "per";
    names << tr("Persian (farsi)");

    codes2l << "pl";
    codes3l << "pol";
    names << tr("Polish");

    codes2l << "pt";
    codes3l << "por";
    names << tr("Portuguese");

    codes2l << "pb";
    codes3l << "pob";
    names << tr("Portuguese-BR");

    codes2l << "ru";
    codes3l << "rus";
    names << tr("Russian");

    codes2l << "ro";
    codes3l << "rum";
    names << tr("Romanian");

    codes2l << "sr";
    codes3l << "scc";
    names << tr("Serbian");

    codes2l << "sl";
    codes3l << "slv";
    names << tr("Slovenian");

    codes2l << "sv";
    codes3l << "swe";
    names << tr("Swedish");

    codes2l << "sk";
    codes3l << "slo";
    names << tr("Slovak");

    codes2l << "tr";
    codes3l << "tur";
    names << tr("Turkish");

    codes2l << "vi";
    codes3l << "vie";
    names << tr("Vietnamese");

    codes2l << "hu";
    codes3l << "hun";
    names << tr("Hungarian");

    codes2l << "it";
    codes3l << "ita";
    names << tr("Italian");
}

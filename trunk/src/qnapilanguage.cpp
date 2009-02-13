/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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
	names << tr("Albański");

	codes2l << "en";
	codes3l << "eng";
	names << tr("Angielski");

	codes2l << "ar";
	codes3l << "ara";
	names << tr("Arabski");

	codes2l << "bg";
	codes3l << "bul";
	names << tr("Bułgarski");

	codes2l << "zh";
	codes3l << "chi";
	names << tr("Chiński");

	codes2l << "hr";
	codes3l << "hrv";
	names << tr("Chorwacki");

	codes2l << "cs";
	codes3l << "cze";
	names << tr("Czeski");

	codes2l << "da";
	codes3l << "dan";
	names << tr("Duński");

	codes2l << "et";
	codes3l << "est";
	names << tr("Estoński");

	codes2l << "fi";
	codes3l << "fin";
	names << tr("Fiński");

	codes2l << "fr";
	codes3l << "fre";
	names << tr("Francuski");

	codes2l << "gl";
	codes3l << "glg";
	names << tr("Galicyjski");

	codes2l << "el";
	codes3l << "ell";
	names << tr("Grecki");

	codes2l << "he";
	codes3l << "heb";
	names << tr("Hebrajski");

	codes2l << "es";
	codes3l << "spa";
	names << tr("Hiszpanski");

	codes2l << "nl";
	codes3l << "dut";
	names << tr("Holenderski");

	codes2l << "id";
	codes3l << "ind";
	names << tr("Indonezyjski");

	codes2l << "ja";
	codes3l << "jpn";
	names << tr("Japoński");

	codes2l << "ko";
	codes3l << "kor";
	names << tr("Koreański");

	codes2l << "mk";
	codes3l << "mac";
	names << tr("Macedoński");

	codes2l << "de";
	codes3l << "ger";
	names << tr("Niemiecki");

	codes2l << "no";
	codes3l << "nor";
	names << tr("Norweski");

	codes2l << "oc";
	codes3l << "oci";
	names << tr("Oksytański");

	codes2l << "fa";
	codes3l << "per";
	names << tr("Perski (farsi)");

	codes2l << "pl";
	codes3l << "pol";
	names << tr("Polski");

	codes2l << "pt";
	codes3l << "por";
	names << tr("Portugalski");

	codes2l << "pb";
	codes3l << "pob";
	names << tr("Portugalski-BR");

	codes2l << "ru";
	codes3l << "rus";
	names << tr("Rosyjski");

	codes2l << "ro";
	codes3l << "rum";
	names << tr("Rumuński");

	codes2l << "sr";
	codes3l << "scc";
	names << tr("Serbski");

	codes2l << "sl";
	codes3l << "slv";
	names << tr("Słoweński");

	codes2l << "sv";
	codes3l << "swe";
	names << tr("Szwedzki");

	codes2l << "sk";
	codes3l << "slo";
	names << tr("Słowacki");

	codes2l << "tr";
	codes3l << "tur";
	names << tr("Turecki");

	codes2l << "vi";
	codes3l << "vie";
	names << tr("Wietnamski");

	codes2l << "hu";
	codes3l << "hun";
	names << tr("Węgierski");

	codes2l << "it";
	codes3l << "ita";
	names << tr("Włoski");
}

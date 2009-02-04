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

#ifndef __QNAPILANGUAGE__H__
#define __QNAPILANGUAGE__H__

#include <QObject>
#include <QStringList>
#include <QDebug>

class QNapiLanguage : public QObject
{
Q_OBJECT

public:

	QNapiLanguage(QString source);

	QString toTwoLetter();
	QString toTriLetter();
	QString toFullName();

	QStringList listLanguages();

private:

	QStringList codes2l, codes3l, names;
	int idx;

	void fill_tables();

};

#endif

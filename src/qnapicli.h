/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

#ifndef __QNAPICLI__H__
#define __QNAPICLI__H__

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <iostream>

#include "version.h"
#include "qnapi.h"
#include "qnapilanguage.h"

class QNapiCli : public QCoreApplication
{
public:

	QNapiCli(int argc, char **argv) :
		QCoreApplication(argc, argv),
		mode(CM_UNSET), showPolicy(SLP_USE_CONFIG) {}

	static bool isCliCall(int argc, char **argv);
	int exec();

private:

	QStringList movieList;

	enum CliMode {
		CM_UNSET, CM_HELP, CM_HELP_LANGUAGES, CM_CONSOLE, CM_QUIET
	};

	enum ShowListPolicy{
		SLP_SHOW, SLP_USE_CONFIG, SLP_NEVER_SHOW
	};

	CliMode mode;
	ShowListPolicy showPolicy;

	QString lang;

	bool analyze();
	void printHeader();
	void printHelp();
	void printHelpLanguages();
	void printCli(const QString & string);

};

#endif

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

#ifndef __QNAPICLI__H__
#define __QNAPICLI__H__

#include <QString>
#include <QStringList>
#include <QFileInfo>

#include "version.h"
#include "qnapiprojektengine.h"

class QNapiCli
{
public:

	QNapiCli(int argc, char **argv)
		: m_argc(argc), m_argv(argv), mode(CM_UNSET) {}

	bool analyze();
	int exec();

private:

	int m_argc;
	char **m_argv;
	QStringList movieList;

	enum CliMode {
		CM_UNSET, CM_HELP, CM_CONSOLE, CM_QUIET
	};
	
	CliMode mode;

	void printHelp();
	void printCli(const QString & string);

};

#endif

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


#include "qnapi.h"


bool QNapi::checkP7ZipPath()
{
	return QFileInfo(GlobalConfig().p7zipPath()).isExecutable();
}


bool QNapi::checkTmpPath()
{
	QFileInfo f(GlobalConfig().tmpPath());
	return f.isDir() && f.isWritable();
}


bool QNapi::ppEnabled()
{
	return GlobalConfig().ppEnabled();
}


bool QNapi::addEngine(QString engine)
{
	if(engine == "NapiProjekt")
	{
		//enginesList << new QNapiProjektEngine();
		return true;
	}
	else if(engine == "OpenSubtitles")
	{
		//enginesList << new QOpenSubtitlesEngine();
		return true;
	}
	else
	{
		errorMsg = QString("Nieobsługiwany silnik pobierania: %1.").arg(engine);
		return false;
	}
}


bool QNapi::addEngines(QStringList engines)
{
	foreach(QString e, engines)
	{
		if(!addEngine(e))
			return false;
	}
	return true;
}

	
void QNapi::setMoviePath(QString path)
{
	movie = path;
}

QString QNapi::moviePath()
{
	return movie;
}

bool QNapi::checkWritePermissions()
{
	return QFileInfo(QFileInfo(movie).path()).isWritable();
}


bool QNapi::lookForSubtitles(QString lang)
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		//e->costam...
	}
	return true;
}


QList<QNapiSubtitleInfo> QNapi::listSubtitles()
{
	return subtitlesList;
}


bool QNapi::download(int i)
{
	QNapiSubtitleInfo s = subtitlesList[i];
	QNapiAbstractEngine *e = engineByName(s.engine);
	//e->blablabla
	return true;
}


bool QNapi::match()
{
	return true;
}


bool QNapi::pp()
{
	
}


void QNapi::cancel()
{
	
}


void QNapi::cleanup()
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		//e->cleanup
	}
}


QString QNapi::error()
{
	return errorMsg;
}

QNapiAbstractEngine * QNapi::engineByName(QString name)
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		if(name == (e->engineName()))
		{
			return e;
		}
	}
	
	return 0;
}

QString QNapi::nameByEngine(QNapiAbstractEngine * engine)
{
	return engine->engineName();
}

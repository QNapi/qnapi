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

QNapi::~QNapi()
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		if(e)
		{
			e->cleanup();
			delete e;
		}
	}
}

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
		enginesList << (new QNapiProjektEngine());
		qDebug() << "addEngine: " << enginesList;
		return true;
	}
	else if(engine == "OpenSubtitles")
	{
//		eginesList << (new QOpenSubtitlesEngine());
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
	currentEngine = 0;
}

QString QNapi::moviePath()
{
	return movie;
}

bool QNapi::checkWritePermissions()
{
	return QFileInfo(QFileInfo(movie).path()).isWritable();
}

void QNapi::checksum()
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		e->checksum(movie);
	}
}

bool QNapi::lookForSubtitles(QString lang)
{
	subtitlesList.clear();

	bool result = false;
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		e->setMoviePath(movie);
		result = e->lookForSubtitles(lang) || result;
	}

	if(!result)
	{
		errorMsg = "Żaden z silników pobierania nie działa!";
	}

	return result;
}


QList<QNapiSubtitleInfo> QNapi::listSubtitles()
{
	int curr_offset = 0;
	
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		QList<QNapiSubtitleInfo> list =  e->listSubtitles();
		offsetsList.insert(nameByEngine(e), curr_offset);
		curr_offset += list.size();
		subtitlesList << list;
	}
	
	return subtitlesList;
}


bool QNapi::download(int i)
{
	QNapiSubtitleInfo s = subtitlesList[i];
	currentEngine = engineByName(s.engine);
	if(!currentEngine) return false;
	int offset = offsetsList.value(s.engine, 0);
	return currentEngine->download(i - offset);
}


bool QNapi::unpack()
{
	return currentEngine ? currentEngine->unpack() : false;
}

bool QNapi::match()
{
	return currentEngine ? currentEngine->match() : false;
}

void QNapi::pp()
{
	if(currentEngine) currentEngine->pp();
}

void QNapi::cleanup()
{
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		e->cleanup();
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

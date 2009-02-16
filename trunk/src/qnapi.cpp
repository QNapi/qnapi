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

QStringList QNapi::enumerateEngines()
{
	QStringList engines;
	engines << "NapiProjekt";
	engines << "OpenSubtitles";
	return engines;
}

bool QNapi::addEngine(QString engine)
{
	if(engine == "NapiProjekt")
	{
		enginesList << (new QNapiProjektEngine());
		return true;
	}
	else if(engine == "OpenSubtitles")
	{
		enginesList << (new QOpenSubtitlesEngine());
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

bool QNapi::lookForSubtitles(QString lang, QString engine)
{
	subtitlesList.clear();

	bool result = false;


	if(engine.isEmpty())
	{
		foreach(QNapiAbstractEngine *e, enginesList)
		{
			e->setMoviePath(movie);
			result = e->lookForSubtitles(lang) || result;
		}
	}
	else
	{
		QNapiAbstractEngine *e = engineByName(engine);
		if(e)
		{
			e->setMoviePath(movie);
			result = e->lookForSubtitles(lang);
		}
	}

	if(!result)
	{
		errorMsg = "Nie znaleziono napisów!";
	}

	return result;
}

QList<QNapiSubtitleInfo> QNapi::listSubtitles()
{
	int curr_offset = 0;
	subtitlesList.clear();

	foreach(QNapiAbstractEngine *e, enginesList)
	{
		QList<QNapiSubtitleInfo> list =  e->listSubtitles();

		offsetsList.insert(nameByEngine(e), curr_offset);
		curr_offset += list.size();
		subtitlesList << list;
	}
	return subtitlesList;
}

bool QNapi::needToShowList()
{
	theBestIdx = 0;

	int i = 0;
	bool foundBestIdx = false;
	foreach(QNapiSubtitleInfo s, listSubtitles())
	{
		if(s.resolution == SUBTITLE_GOOD)
		{
			theBestIdx = i;
			foundBestIdx = true;
			break;
		}
		++i;
	}

	if(listSubtitles().size() <= 1)
		return false;

	if(GlobalConfig().downloadPolicy() == DP_ALWAYS_SHOW_LIST)	
		return true;
	if(GlobalConfig().downloadPolicy() == DP_NEVER_SHOW_LIST)	
		return false;
	return !foundBestIdx;

}

int QNapi::bestIdx()
{
	return theBestIdx;
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

QStringList QNapi::listLoadedEngines()
{
	QStringList list;
	foreach(QNapiAbstractEngine *e, enginesList)
	{
		list << e->engineName();
	}
	return list;
}

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


void QNapi::addEngine(QString engine)
{
	
}


void QNapi::addEngines(QStringList engines)
{
	
}

	
void QNapi::setMoviePath(QString path)
{
	
}


bool QNapi::checkWritePermissions()
{
	
}


bool QNapi::lookForSubtitles(QString lang)
{
	
}


QList<QNapiSubtitleInfo> QNapi::listSubtitles()
{
	
}


bool QNapi::download(int i)
{
	
}


bool QNapi::match()
{
	
}


bool QNapi::pp()
{
	
}


void QNapi::cancel()
{
	
}


void QNapi::cleanup()
{
	
}


QString QNapi::error()
{
	return errorMsg;
}


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

#ifndef __QNAPIABSTRACTENGINE__H__
#define __QNAPIABSTRACTENGINE__H__

#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>

#include "qnapiconfig.h"
#include "multipartdata.h"

class QNapiAbstractEngine
{
protected:

	QString moviePath, subtitlesPath, tmpPath, checkSum;

	QNapiAbstractEngine(const QString & movieFile, const QString & subtitlesFile = "") 
		: moviePath(movieFile), subtitlesPath(subtitlesFile)
	{
		tmpPath = GlobalConfig().tmpPath();
	};

	virtual ~QNapiAbstractEngine()
	{
		cleanup();
	};

	virtual QString checksum() = 0;
	virtual bool tryDownload() = 0;
	virtual bool tryMatch() = 0;
	virtual void cleanup() {};

public:

	static bool checkP7ZipPath();
	static bool checkTmpPath();
	bool ppChangeSubtitlesEncoding(const QString & from, const QString & to);
	bool ppChangeSubtitlesEncoding(const QString & to);
	bool ppChangeSubtitlesPermissions(QFile::Permissions permissions);
};

#endif

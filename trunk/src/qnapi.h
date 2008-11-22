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

#ifndef __QNAPI__H__
#define __QNAPI__H__


#include <QtCore>
#include "qnapiconfig.h"
#include "qnapiabstractengine.h"

struct QNapiSubtitleInfo
{
	QString lang;
	QString engine;
	QString url;
};

class QNapi
{
	public:

		static bool checkP7ZipPath();
		static bool checkTmpPath();
		static bool ppEnabled();

		void addEngine(QString engine);
		void addEngines(QStringList engines);
	
		void setMoviePath(QString path);
		bool checkWritePermissions();
		bool lookForSubtitles(QString lang);
		QList<QNapiSubtitleInfo> listSubtitles();

		bool download(int i);
		bool match();
		bool pp();
		void cancel();

		void cleanup();
		QString error();

	private:

		QString moviePath;
		QString errorMsg;
		QList<QNapiAbstractEngine*> enginesList;
		QList<QNapiSubtitleInfo> subtitlesList;

	signals:

		void listSubtitlesReady();
		void downloadFinished(bool result);

};


#endif // __QNAPI__H__

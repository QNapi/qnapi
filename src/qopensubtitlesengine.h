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

#ifndef __QOPENSUBTITLESENGINE__H__
#define __QOPENSUBTITLESENGINE__H__

#include <QUrl>
#include <QDir>

#include "qnapiabstractengine.h"
#include "xmlrpc/qsyncxmlrpcclient.h"
#include "synchttp.h"
#include "version.h"

const QString openSubtitlesXmlRpcHost  = "www.opensubtitles.org";
const QString openSubtitlesXmlRpcPath  = "/xml-rpc";
const int openSubtitlesXmlRpcPort  = 80;

class QOpenSubtitlesEngine : public QNapiAbstractEngine
{
public:

	QOpenSubtitlesEngine(const QString & movieFile = "", const QString & subtitlesFile = "");
	~QOpenSubtitlesEngine();

	// zwraca nazwe modulu
	QString engineName();
	// zwraca informacje nt. modulu
	QString engineInfo();
	// zwraca ikone silnika pobierania
	QIcon engineIcon();

	// oblicza sume kontrolna pliku filmowego
	QString checksum(QString filename = "");
	// szuka napisow
	bool lookForSubtitles(QString lang);
	// wyniki wyszukiwania
	QList<QNapiSubtitleInfo> listSubtitles();
	// probuje pobrac napisy
	bool download(int idx);
	// probuje rozpakowac napisy
	bool unpack();
	// czysci smieci, pliki tymczasowe
	void cleanup();

private:

	QString p7zipPath, lang, tmpPackedFile, subFileName;
	quint64 fileSize;

	QSyncXmlRpcClient rpc;
	QString token;

	QList<QNapiSubtitleInfo> subtitlesList;

	// sprawdza czy dana instancja klasy jest zalogowana na sewerze
	bool isLogged() { return !token.isEmpty(); }
	// loguje na serwer OpenSubtitles
	bool login();
	// wylogowuje z serwera
	void logout();

};

#endif

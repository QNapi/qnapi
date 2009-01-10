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

#ifndef __QNAPIPROJEKTENGINE__H__
#define __QNAPIPROJEKTENGINE__H__

#include <QCryptographicHash>
#include <QUrl>
#include <QMessageBox>

#ifdef Q_WS_WIN // for SetFileAttributes
#include <windows.h>
#endif

#include <cmath>

#include "qnapiabstractengine.h"
#include "qmultiparthttprequest.h"
#include "synchttp.h"
#include "movieinfo.h"

const unsigned long NAPI_10MB = 10485760;

const QString napiZipPassword = "iBlm8NTigvru0Jr0";
const QString napiDownloadUrlTpl= "http://www.napiprojekt.pl/unit_napisy/dl.php"
				"?l=%1&f=%2&t=%3&v=other&kolejka=false&napios=Linux&nick=%4&pass=%5";
const QString napiCheckUserUrlTpl = "http://www.napiprojekt.pl/users_check.php?nick=%1&pswd=%2";
const QString napiUploadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/upload.php"
				"?m_length=%1&m_resolution=%2x%3&m_fps=%4&m_hash=%5&m_filesize=%6";
const QString napiUploadUrlSimpleTpl = "http://www.napiprojekt.pl/unit_napisy/upload.php"
				"?m_hash=%5&m_filesize=%6";
const QString napiReportBadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/zlenapisyadd.php";
const QString napiCreateUserUrlTpl = "http://www.napiprojekt.pl/users_add.php";

class QNapiProjektEngine : public QNapiAbstractEngine
{
public:

	enum UploadResult
	{
		NAPI_ADDED_NEW, NAPI_OK, NAPI_FAIL, NAPI_UNKNOWN
	};

	enum ReportResult
	{
		NAPI_REPORTED, NAPI_NO_SUBTITLES, NAPI_NOT_REPORTED
	};

	QNapiProjektEngine(const QString & movieFile = "", const QString & subtitlesFile = "");
	~QNapiProjektEngine();

	// zwraca nazwe modulu
	QString engineName();
	// zwraca informacje nt. modulu
	QString engineInfo();
	// zwraca ikone silnika pobierania
	QIcon engineIcon();
	// zwraca czy silnik jest konfigurowalny
	bool isConfigurable();
	// wywoluje okienko konfiguracji
	void configure(QWidget * parent);

	QString checksum(QString filename = "");
	bool lookForSubtitles(QString lang);
	QList<QNapiSubtitleInfo> listSubtitles();
	bool download(int idx);
	bool unpack();
	void cleanup();

	QString name()
	{
		return QString("NapiProjekt");
	}

	static bool createUser(const QString & nick, const QString & pass,
							const QString & email, QString * response);
	static bool checkUser(const QString & nick, const QString & pass);
	UploadResult uploadSubtitles(const QString & language, const QString & nick,
									const QString & pass, bool correct = false,
									const QString & comment = "");
	ReportResult reportBad(const QString & language, const QString & nick, const QString & pass,
							const QString & comment, QString *response);

private:

	QString p7zipPath, nick, pass, tmpPackedFile;

	QList<QNapiSubtitleInfo> subtitlesList;

	QString checksum(QString filename, bool limit10M);
	QString npFDigest(const QString & input);

};

#endif

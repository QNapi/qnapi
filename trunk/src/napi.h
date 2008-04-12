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

#ifndef __NAPI__H__
#define __NAPI__H__

#include <QString>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QHttp>
#include <QUrl>
#include <QProcess>
#include <QTextCodec>
#include <QTextStream>
#include <cstdlib>
#include <cmath>
#include "synchttp.h"
#include "multipartdata.h"
#include "movieinfo.h"
#include "version.h"
#include "qnapiconfig.h"

#ifdef Q_WS_WIN // for SetFileAttributes
#include <windows.h>
#endif

const QString napiZipPassword = "iBlm8NTigvru0Jr0";
const QString napiDownloadUrlTpl= "http://www.napiprojekt.pl/unit_napisy/dl.php"
				"?l=%1&f=%2&t=%3&v=other&kolejka=false&napios=Linux&nick=%4&pass=%5";
const QString napiCheckUserUrlTpl = "http://www.napiprojekt.pl/users_check.php?nick=%1&pswd=%2";
const QString napiUploadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/upload.php"
				"?m_length=%1&m_resolution=%2x%3&m_fps=%4&m_hash=%5&m_filesize=%6";
const QString napiReportBadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/zlenapisyadd.php";
const QString napiCreateUserUrlTpl = "http://www.napiprojekt.pl/users_add.php";
const unsigned long NAPI_10MB = 10485760;

enum napiUploadResult
{
	NAPI_ADDED_NEW, NAPI_OK, NAPI_FAIL, NAPI_UNKNOWN
};

enum napiReportResult
{
	NAPI_REPORTED, NAPI_NO_SUBTITLES, NAPI_NOT_REPORTED
};


QString napiFileMd5Sum(const QString & filename, unsigned long max_size = 0);

QString napiFDigest(QString input);

bool napiDownload(const QString & md5sum, const QString & path, const QString & language = "PL",
					const QString & nick = "", const QString & pass = "");

bool napiMatchSubtitles(const QString & md5sum, const QString & zip_file, const QString & movie_file,
						bool noBackup = false, const QString & tmp_path = "/tmp",
						const QString & p7zip_path = "/usr/bin/7z");

bool napiCheckUser(const QString & nick, const QString & pass);

napiUploadResult napiUploadSubtitles(const QString & movie_file, const QString & subtitles_file,
										const QString & language, const QString & nick,
										const QString & pass, bool correct = false,
										const QString & comment = "",
										const QString & tmp_path = "/tmp",
										const QString & p7zip_path = "/usr/bin/7z");

napiReportResult napiReportBad(const QString & movie_file, const QString & language,
								const QString & nick, const QString & pass, const QString & comment,
								QString *response);

bool napiConvertFile(const QString & file, const QString & enc_from, const QString & enc_to);
bool napiConvertFile(const QString & file, const QString & enc_to);

bool napiCreateUser(const QString & nick, const QString & pass, const QString & email,
					QString * response);

bool napiCheck7Zip();
bool napiCheckTmpPath();

#endif

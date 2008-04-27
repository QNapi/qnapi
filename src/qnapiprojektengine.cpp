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

#include "qnapiprojektengine.h"

// konstruktor klasy
QNapiProjektEngine::QNapiProjektEngine(const QString & movieFile, const QString & subtitlesFile)
	: QNapiAbstractEngine(movieFile, subtitlesFile)
{
	p7zipPath = GlobalConfig().p7zipPath();
	lang = GlobalConfig().language();
	nick = GlobalConfig().nick();
	pass = GlobalConfig().pass();
	noBackup = GlobalConfig().noBackup();
	tmpFile =  tmpPath + "/QNapi.napisy.7z";
}

// destruktor klasy
QNapiProjektEngine::~QNapiProjektEngine()
{
	cleanup();
}

// oblicza sume kontrolna dla pliku filmowego (md5 z pierwszych 10MB pliku)
QString QNapiProjektEngine::checksum(const QString & filename, bool limit10M)
{
	QFile file(filename);
	QByteArray fileArray;

	if(!file.open(QIODevice::ReadOnly))
		return QString("");

	fileArray = limit10M ? file.read(NAPI_10MB) : file.readAll();
	file.close();

	QByteArray b = QCryptographicHash::hash(fileArray, QCryptographicHash::Md5);
	fileArray.clear();

	QString out;
	char next[3];

	for(int i = 0; i < 16; i++)
	{
		sprintf(next, "%.2x", (unsigned char)b[i]);
		out += next;
	}
	
	checkSum = out;
	return checkSum;
}

QString QNapiProjektEngine::checksum()
{
	return checksum(moviePath);
}

// Probuje pobrac napisy do filmu z serwera NAPI
bool QNapiProjektEngine::tryDownload()
{
	if(checkSum.isEmpty()) checksum();
	
	SyncHTTP http;
	QString urlTxt = napiDownloadUrlTpl.arg(lang).arg(checkSum).arg(napiFDigest(checkSum)).arg(nick).arg(pass);

	QUrl url(urlTxt);

	http.setHost(url.host());
	http.syncGet(url.path() + "?" + url.encodedQuery());

	QByteArray buffer = http.readAll();

	if(buffer.indexOf("NPc") == 0)
		return false;

	QFile file(tmpFile);
	if(file.exists()) file.remove();
	if(!file.open(QIODevice::WriteOnly))
		return false;

	int r = file.write(buffer);
	file.close();
	return (bool)r;
}

// Probuje dopasowac napisy do filmu
bool QNapiProjektEngine::tryMatch()
{
	if(!QFile::exists(tmpFile)) return false;
	if(!QFile::exists(moviePath)) return false;
	if(!checkP7ZipPath()) return false;

	QStringList args;
	args << "x" << "-y" << ("-p" + napiZipPassword) << ("-o" + tmpPath) << tmpFile;

	QProcess p7zip;
	p7zip.start(p7zipPath, args);

	if(!p7zip.waitForFinished()) return false;

	QString subtitleFile = tmpPath + "/" + checkSum + ".txt";
	if(!QFile::exists(subtitleFile)) return false;

	QFileInfo mf(moviePath);

	QString newName = mf.path() + "/" + mf.completeBaseName() + ".txt";

	if(QFile::exists(newName))
	{
		if(!noBackup)
		{
			QFileInfo nn(newName);
			QFile::copy(newName, nn.path() + "/" + nn.completeBaseName() + "_kopia.txt");
		}
		QFile::remove(newName);
	}

	bool r;

	#ifdef Q_WS_WIN
	// Pod windowsem, aby "wyczyscic" atrybuty pliku, tworzymy go na nowo
	QFile f(newName), f2(subtitleFile);
	if(!f.open(QIODevice::WriteOnly) || !f2.open(QIODevice::ReadOnly))
	{
		r = false;
		f.close();
	}
	else
	{
		r = f.write(f2.readAll()) > 0;
		f2.close();
		f.close();
	}
	#else
	// pod normalnymi OS-ami nie trzeba sie gimnastykowac z atrybutami
	r = QFile::copy(subtitleFile, newName);
	#endif

	subtitlesPath = newName;

	QFile::remove(subtitleFile);
	QFile::remove(tmpFile);

	return r;
}

void QNapiProjektEngine::cleanup()
{
	QFile::remove(tmpFile);
}

// Tworzy konto uzytkownika na serwerze NAPI
bool QNapiProjektEngine::createUser(const QString & nick, const QString & pass,
									const QString & email, QString * response)
{
	QMultipartHttpRequest postData;

	postData.addBoundary();
	postData.addContentDisposition("name=\"login\"");
	postData.addData(nick);

	postData.addBoundary();
	postData.addContentDisposition("name=\"haslo\"");
	postData.addData(pass);

	postData.addBoundary();
	postData.addContentDisposition("name=\"mail\"");
	postData.addData(email);

	postData.addBoundary();
	postData.addContentDisposition("name=\"z_programu\"");
	postData.addData(QString("true"));

	postData.addEndingBoundary();

	QByteArray data = postData.requestStream();

	QUrl url(napiCreateUserUrlTpl);

	QHttpRequestHeader header("POST", url.path());

	header.setValue("Host", url.host());
	header.setValue("Accept", "text/html, */*");
	header.setValue("Content-Type", "multipart/form-data; boundary=" + postData.boundaryTxt());
	header.setValue("Connection", "keep-alive");
	header.setValue("User-Agent", QString("QNapi ") + QNAPI_VERSION);

	SyncHTTP http;
	http.setHost(url.host());
	if(!http.syncRequest(header, data))
		return false;

	*response = QTextCodec::codecForName("windows-1250")->toUnicode(http.readAll());

	return true;
}

// Sprawdza uzytkownika w bazie
bool QNapiProjektEngine::checkUser(const QString & nick, const QString & pass)
{
	SyncHTTP http;
	QString urlTxt = napiCheckUserUrlTpl.arg(nick).arg(pass);

	QUrl url(urlTxt);
	http.setHost(url.host());
	http.syncGet(url.path() + "?" + url.encodedQuery());

	QString buffer = http.readAll();
	if(buffer.indexOf("ok") == 0) return true;
	return false;
}

// Wrzuca napisy do bazy NAPI
QNapiProjektEngine::UploadResult
	QNapiProjektEngine::uploadSubtitles(const QString & language, const QString & nick,
										const QString & pass, bool correct, const QString & comment)
{
	if(!QFile::exists(moviePath) || !QFile::exists(subtitlesPath))
		return NAPI_FAIL;

	MovieInfo movieInfo(moviePath);
	if(movieInfo.isErr)
		return NAPI_FAIL;

	unsigned long movie_size = QFileInfo(moviePath).size();
	QString movie_md5 = checksum(moviePath);
	QString subtitles_md5 = checksum(subtitlesPath, false);

	QString newSubtitlesName = tmpPath + "/" + movie_md5 + ".txt";
	if(QFile::exists(newSubtitlesName)) QFile::remove(newSubtitlesName);
	if(!QFile::copy(subtitlesPath, newSubtitlesName))
		return NAPI_FAIL;

	QString zipFileName = QFileInfo(newSubtitlesName).path() + "/"
							+ QFileInfo(newSubtitlesName).completeBaseName() + ".zip";

	QStringList args;
	args << "a" << "-l" << "-y" << zipFileName << ("-p"+napiZipPassword) << newSubtitlesName;

	QProcess p7zip;
	p7zip.start(p7zipPath, args);

	if(!p7zip.waitForFinished())
		return NAPI_FAIL;

	QFile::remove(newSubtitlesName);

	if(!QFile::exists(zipFileName))
		return NAPI_FAIL;

	// Przygotowujemy dane do zapytania POST
	QMultipartHttpRequest postData;

	postData.addBoundary();
	postData.addContentDisposition("name=\"kmt\"");
	postData.addData(comment);

	postData.addBoundary();
	postData.addContentDisposition("name=\"poprawka\"");
	postData.addData(QString(correct ? "true" : "false"));

	postData.addBoundary();
	postData.addContentDisposition("name=\"v\"");
	postData.addData(QString("other"));

	postData.addBoundary();
	postData.addContentDisposition("name=\"l\"");
	postData.addData(language);

	postData.addBoundary();
	postData.addContentDisposition("name=\"t\"");
	postData.addData(napiFDigest(movie_md5));

	postData.addBoundary();
	postData.addContentDisposition("name=\"m_filename\"");
	postData.addData(QFileInfo(moviePath).fileName());

	postData.addBoundary();
	postData.addContentDisposition("name=\"nick\"");
	postData.addData(nick);

	postData.addBoundary();
	postData.addContentDisposition("name=\"pass\"");
	postData.addData(pass);

	postData.addBoundary();
	postData.addContentDisposition("name=\"s_hash\"");
	postData.addData(subtitles_md5);

	postData.addBoundary();
	postData.addContentDisposition("name=\"MAX_FILE_SIZE\"");
	postData.addData(QString("512000"));

	postData.addBoundary();
	postData.addContentDisposition("name=\"plik\"; filename=\"" +
		QFileInfo(zipFileName).fileName() + "\"");
	postData.addContentType("subtitles/zip");

	QFile fZip(zipFileName);
	if(!fZip.open(QIODevice::ReadOnly))
		return NAPI_FAIL;

	postData.addData(fZip.readAll());
	postData.addEndingBoundary();

	fZip.close();
	QFile::remove(zipFileName);

	QByteArray data = postData.requestStream();

	QString movie_fps = QString::number((int)ceil(movieInfo.fps * 100));
	movie_fps.insert(2, ',');

	QString urlTxt = napiUploadUrlTpl.arg(movieInfo.time).arg(movieInfo.width).arg(movieInfo.height)
						.arg(movie_fps).arg(movie_md5).arg(movie_size);

	QUrl url(urlTxt);

	QHttpRequestHeader header("POST", url.path() + "?" + url.encodedQuery());

	header.setValue("Host", url.host());
	header.setValue("Accept", "text/html, */*");
	header.setValue("Content-Type", "multipart/form-data; boundary=" + postData.boundaryTxt());
	header.setValue("Connection", "keep-alive");
	header.setValue("User-Agent", QString("QNapi ") + QNAPI_VERSION);

	SyncHTTP http;
	http.setHost(url.host());
	if(!http.syncRequest(header, data))
		return NAPI_FAIL;

	QString response = http.readAll();

	if(response.indexOf("NPc0") == 0)
		return NAPI_ADDED_NEW;
	if((response.indexOf("NPc2") == 0) || (response.indexOf("NPc3") == 0))
		return NAPI_OK;
	return NAPI_UNKNOWN;
}

// Wysyla blad o niepasujacyh napisach
QNapiProjektEngine::ReportResult
	QNapiProjektEngine::reportBad(const QString & language, const QString & nick, const QString & pass,
									const QString & comment, QString *response)
{
	QFileInfo fi(moviePath);
	subtitlesPath = fi.path() + "/" + fi.completeBaseName() + ".txt";

	if(!QFile::exists(subtitlesPath))
		return NAPI_NO_SUBTITLES;

	QMultipartHttpRequest postData;

	postData.addBoundary();
	postData.addContentDisposition("name=\"nick\"");
	postData.addData(nick);

	postData.addBoundary();
	postData.addContentDisposition("name=\"pass\"");
	postData.addData(pass);

	postData.addBoundary();
	postData.addContentDisposition("name=\"l\"");
	postData.addData(language);

	postData.addBoundary();
	postData.addContentDisposition("name=\"md5\"");
	postData.addData(checkSum);

	postData.addBoundary();
	postData.addContentDisposition("name=\"kmt\"");
	postData.addData(comment);
	postData.addEndingBoundary();

	QByteArray data = postData.requestStream();

	QUrl url(napiReportBadUrlTpl);

	QHttpRequestHeader header("POST", url.path());

	header.setValue("Host", url.host());
	header.setValue("Accept", "text/html, */*");
	header.setValue("Content-Type", "multipart/form-data; boundary=" + postData.boundaryTxt());
	header.setValue("Connection", "keep-alive");
	header.setValue("User-Agent", QString("QNapi ") + QNAPI_VERSION);

	SyncHTTP http;
	http.setHost(url.host());
	if(!http.syncRequest(header, data))
		return NAPI_NOT_REPORTED;

	*response = QTextCodec::codecForName("windows-1250")->toUnicode(http.readAll());

	return NAPI_REPORTED;
}

// Tajemnicza funkcja f() :D
QString QNapiProjektEngine::napiFDigest(const QString & input)
{
	if(input.size() != 32) return "";

	int idx[] = {0xe, 0x3, 0x6, 0x8, 0x2},
		mul[] = {2, 2, 5, 4, 3},
		add[] = {0x0, 0xd, 0x10, 0xb, 0x5},
		a, m, i, t, v;

	QString b;
	char tmp[2] = { 0, 0 };

	for(int j = 0; j <= 4; j++)
	{
		a = add[j];
		m = mul[j];
		i = idx[j];

		tmp[0] = input[i].toAscii();
		t = a + (int)(strtol(tmp, NULL, 16));
		v = (int)(strtol(input.mid(t, 2).toLocal8Bit(), NULL, 16));

		sprintf(tmp, "%x", (v * m) % 0x10);
		b += tmp;
	}

	return b;
}

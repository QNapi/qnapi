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

#include "napi.h"

// Zwraca sume md5 z pierwszych max_size bajtow z pliku
// lub calego, jesli max_size == 0
QString napiFileMd5Sum(const QString & filename, unsigned long max_size)
{
	QFile file(filename);
	QByteArray fileArray;

	file.open(QIODevice::ReadOnly);
	fileArray = ( max_size == 0 ) ? file.readAll() : file.read(max_size);
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

	return out;
}

// Tajemnicza funkcja f() :D
QString napiFDigest(QString input)
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

// Zasysa napisy o sumie md5sum w jezyku language i zapisuje je w pliku path
bool napiDownload(const QString & md5sum, const QString & path, const QString & language,
							const QString & nick, const QString & pass)
{
	SyncHTTP http;
	QString urlTxt = napiDownloadUrlTpl.arg(language).arg(md5sum).arg(napiFDigest(md5sum)).arg(nick).arg(pass);

	QUrl url(urlTxt);

	http.setHost(url.host());
	http.syncGet(url.path() + "?" + url.encodedQuery());

	QByteArray buffer = http.readAll();

	if(buffer.indexOf("NPc") == 0)
		return false;

	QFile file(path);
	if(file.exists()) file.remove();
	if(!file.open(QIODevice::WriteOnly))
		return false;

	int r = file.write(buffer);
	file.close();
	return r;
}

// Dopasowuje napisy i usuwa pliki tymczasowe
bool napiMatchSubtitles(const QString & md5sum, const QString & zip_file, const QString & movie_file,
						bool noBackup, const QString & tmp_path, const QString & p7zip_path)
{
	if(!QFile::exists(zip_file)) return false;
	if(!QFile::exists(movie_file)) return false;
	QFileInfo fi(p7zip_path);
	if(!fi.isExecutable()) return false;

	QStringList args;
	args << "x" << "-y" << ("-p"+napiZipPassword) << ("-o"+tmp_path) << zip_file;

	QProcess p7zip;
	p7zip.start(p7zip_path, args);

	if(!p7zip.waitForFinished()) return false;

	QString subtitleFile = tmp_path + "/" + md5sum + ".txt";
	if(!QFile::exists(subtitleFile)) return false;

	QFileInfo mf(movie_file);

	QString newName = mf.path() + "/" + mf.completeBaseName() + ".txt";

	if(QFile::exists(newName))
	{
		if(!noBackup)
		{
			QFileInfo nn(newName);
			QFile::copy(newName, nn.path() + "/" + nn.completeBaseName() + "_old.txt");
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

	QFile::remove(subtitleFile);
	QFile::remove(zip_file);

	return r;
}

// Sprawdza uzytkownika w bazie
bool napiCheckUser(const QString & nick, const QString & pass)
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

// Wysyla napisy do bazy
napiUploadResult napiUploadSubtitles(const QString & movie_file, const QString & subtitles_file,
										const QString & language, const QString & nick,
										const QString & pass, bool correct, const QString & comment,
										const QString & tmp_path, const QString & p7zip_path)
{
	if(!QFile::exists(movie_file) || !QFile::exists(subtitles_file))
		return NAPI_FAIL;

	MovieInfo movieInfo(movie_file);
	if(movieInfo.isErr)
		return NAPI_FAIL;

	unsigned long movie_size = QFileInfo(movie_file).size();
	QString movie_md5 = napiFileMd5Sum(movie_file, NAPI_10MB);
	QString subtitles_md5 = napiFileMd5Sum(subtitles_file, NAPI_10MB);

	QString newSubtitlesName = tmp_path + "/" + movie_md5 + ".txt";
	if(QFile::exists(newSubtitlesName)) QFile::remove(newSubtitlesName);
	if(!QFile::copy(subtitles_file, newSubtitlesName))
		return NAPI_FAIL;

	QString zipFileName = QFileInfo(newSubtitlesName).path() + "/"
							+ QFileInfo(newSubtitlesName).completeBaseName() + ".zip";

	QStringList args;
	args << "a" << "-l" << "-y" << zipFileName << ("-p"+napiZipPassword) << newSubtitlesName;

	QProcess p7zip;
	p7zip.start(p7zip_path, args);

	if(!p7zip.waitForFinished())
		return NAPI_FAIL;

	QFile::remove(newSubtitlesName);

	if(!QFile::exists(zipFileName))
		return NAPI_FAIL;

	// Przygotowujemy dane do zapytania POST
	MultipartData postData;

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
	postData.addData(QFileInfo(movie_file).fileName());

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

// Zglasza raport o niepasujacych napisach
napiReportResult napiReportBad(const QString & movie_file, const QString & language,
								const QString & nick, const QString & pass, const QString & comment,
								QString *response)
{
	QString subtitles_file = QFileInfo(movie_file).path() + "/"
								+ QFileInfo(movie_file).completeBaseName() + ".txt";

	if(!QFile::exists(subtitles_file))
		return NAPI_NO_SUBTITLES;

	MultipartData postData;

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
	postData.addData(napiFileMd5Sum(movie_file, NAPI_10MB));

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

// Konwertuje napisy z jednego kodowania na inne
bool napiConvertFile(const QString & file, const QString & enc_from, const QString & enc_to)
{
	QFile f(file);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QByteArray fileContent = f.readAll();
	QString contentStr = QTextCodec::codecForName(qPrintable(enc_from))->toUnicode(fileContent);
	f.close();

	if(enc_to.compare("UTF-8", Qt::CaseInsensitive))
	{
		fileContent = QTextCodec::codecForName(qPrintable(enc_to))
						->fromUnicode(contentStr.constData(), contentStr.size());
	}

	if(!f.open(QIODevice::WriteOnly))
		return false;

	f.write(fileContent);
	f.close();

	return true;
}

// Robi to samo co powyzsza funkcja, z tym ze stara sie automatycznie wykryc kodowanie
// zrodlowe konwertowanego pliku
bool napiConvertFile(const QString & file, const QString & enc_to)
{
	QString enc_from;
	QStringList codecs;
	// Tylko takie kodowania obsługuja polskie litery
	codecs << "windows-1250" << "windows-1257" << "ISO-8859-2"
			<< "ISO-8859-13" << "ISO-8859-16" << "UTF-8";

	QFile f(file);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QByteArray testData = f.read(5000);

	f.close();

	foreach(QString codec, codecs)
	{
		QTextStream ts(testData);
		ts.setCodec(qPrintable(codec));
		QString encodedData = ts.readAll();
		QStringList chars = QString::fromUtf8("ą/ś/ż/ć/ń/ł/ó/ę").split("/");

		int i;
		for (i = 0; i < chars.count(); i++)
		{
			if(!encodedData.contains(chars[i], Qt::CaseInsensitive))
				break;
		}

		if(i == chars.count())
		{
			enc_from = codec;
			break;
		}
	}

	if(enc_from.isEmpty())
		return false;

	return napiConvertFile(file, enc_from, enc_to);
}

// Zakladanie kont uzytkownikow na serwerze napi
bool napiCreateUser(const QString & nick, const QString & pass, const QString & email,
					QString * response)
{
	MultipartData postData;

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

// Sprawdza poprawnosc sciezki do 7zipa (z konfiguracji)
bool napiCheck7Zip()
{
	return QFileInfo(GlobalConfig().p7zipPath()).isExecutable();
}

// Sprawdza poprawnosc sciezki do katalogu tymczasowego (z konfiguracji)
bool napiCheckTmpPath()
{
	QFileInfo f(GlobalConfig().tmpPath());
	return f.isDir() && f.isWritable();
}

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

#include "qnapiabstractengine.h"

#include <QFlags>

// Sprawdza poprawnosc sciezki do 7zipa (z konfiguracji)
bool QNapiAbstractEngine::checkP7ZipPath()
{
	return QFileInfo(GlobalConfig().p7zipPath()).isExecutable();
}

// Sprawdza poprawnosc sciezki do katalogu tymczasowego (z konfiguracji)
bool QNapiAbstractEngine::checkTmpPath()
{
	QFileInfo f(GlobalConfig().tmpPath());
	return f.isDir() && f.isWritable();
}

// Sprawdza uprawnienia zapisu do katalogu docelowego (katalogu z filmem)
bool QNapiAbstractEngine::checkWritePermissions()
{
	return QFileInfo(QFileInfo(moviePath).path()).isWritable();
}

// Dokonuje post-przetwarzania pliku z napisami (na podstawie konfiguracji)
void QNapiAbstractEngine::doPostProcessing()
{
	// Usuwanie linii z plikow z napisami
	if(GlobalConfig().ppRemoveLines())
	{
		ppRemoveLinesContainingWords(GlobalConfig().ppRemoveWords());
	}

	// Zmiana kodowania pobranych napisow
	if(GlobalConfig().ppChangeEncoding())
	{
		// Jesli automatycznie nie uda mu sie wykryc kodowania, to jako kodowania
		// zrodlowego uzywa kodowania wybranego przez uzytkownika
		if (!GlobalConfig().ppAutoDetectEncoding()
			|| !ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingTo()))
		{
			ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingFrom(),
										GlobalConfig().ppEncodingTo());
		}
	}

#ifndef Q_WS_WIN
	// Zmiana uprawnien do pliku
	if(GlobalConfig().ppChangePermissions())
	{
		bool validPermissions;
		int permInt = GlobalConfig().ppPermissions().toInt(&validPermissions, 8);

		if(validPermissions)
		{
			int perm = 0;
			perm |= (permInt & 0700) << 2;
			perm |= (permInt & 0070) << 1;
			perm |= (permInt & 0007);
			ppChangeSubtitlesPermissions(QFile::Permissions(perm));
		}
	}
#endif
}

QString QNapiAbstractEngine::ppDetectEncoding(const QString & fileName, int testBufferSize)
{
	QString from;
	QStringList codecs;

	// Tylko takie kodowania obsługuja polskie litery
	codecs << "windows-1250" << "windows-1257" << "ISO-8859-2"
			<< "ISO-8859-13" << "ISO-8859-16" << "UTF-8";

	QFile f(fileName);
	if(!f.open(QIODevice::ReadOnly))
		return "";

	QByteArray testData = (testBufferSize > 0) ? f.read(testBufferSize) : f.readAll();

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
			from = codec;
			break;
		}
	}

	return from;
}

// Konwertuje napisy z jednego kodowania na inne
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & from, const QString & to)
{
	QFile f(subtitlesPath);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QByteArray fileContent = f.readAll();
	QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
	f.close();

	if(to.compare("UTF-8", Qt::CaseInsensitive))
	{
		fileContent = QTextCodec::codecForName(qPrintable(to))
						->fromUnicode(contentStr.constData(), contentStr.size());
	}

	if(!f.open(QIODevice::WriteOnly))
		return false;

	f.write(fileContent);
	f.close();

	return true;
}

// Konwertuje napisy z jednego kodowania na inne, dokonujac proby autodetekcji
// kodowania zrodlowego
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & to)
{
	if(!QFileInfo(subtitlesPath).exists())
		return false;

	QString from = ppDetectEncoding(subtitlesPath);

	if(from.isEmpty())
		return false;

	return ppChangeSubtitlesEncoding(from, to);
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::ppRemoveLinesContainingWords(const QStringList & wordList)
{
	if(!QFileInfo(subtitlesPath).exists())
		return false;

	QString fromCodec = ppDetectEncoding(subtitlesPath);

	if(fromCodec.isEmpty())
		fromCodec = GlobalConfig().ppEncodingFrom();

	QFile f(subtitlesPath);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QList<QByteArray> lines = f.readAll().split('\n');
	QList<QByteArray> out;

	foreach(QByteArray line, lines)
	{
		int i;
		while((i = line.indexOf('\r')) >= 0)
			line.remove(i, 1);

		QTextStream ts(line);
		ts.setCodec(qPrintable(fromCodec));
		QString encLine = ts.readAll();

		if(encLine.isEmpty()) continue;

		bool found = false;
		foreach(QString word, wordList)
		{
			if(encLine.contains(word, Qt::CaseInsensitive))
			{
				found = true;
				break;
			}
		}

		if(found) continue;

		out << line;
	}

	f.close();

	if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	foreach(QByteArray line, out)
	{
		f.write(line);
		f.write("\r\n", 2);
	}
	f.close();

	return true;
}

// Zmienia uprawnienia do pliku z napisami
bool QNapiAbstractEngine::ppChangeSubtitlesPermissions(QFile::Permissions permissions)
{
	if(!QFileInfo(subtitlesPath).exists())
		return false;

	return QFile::setPermissions(subtitlesPath, permissions);
}

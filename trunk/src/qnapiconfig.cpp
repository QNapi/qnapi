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

#include "qnapiconfig.h"

QNapiConfig::QNapiConfig()
{
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
}

QNapiConfig::~QNapiConfig()
{
	if(settings) delete settings;
}

void QNapiConfig::reload()
{
	if(settings) delete settings;
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qnapi");
}

void QNapiConfig::save()
{
	settings->setValue("qnapi/version", QNAPI_VERSION);
	settings->setValue("qnapi/firstrun", false);
	settings->sync();
}

bool QNapiConfig::firstRun()
{
	return settings->value("qnapi/firstrun", true).toBool();
}

QString QNapiConfig::version()
{
	return settings->value("qnapi/version", "").toString();
}

QString QNapiConfig::p7zipPath()
{
	// Odczytujemy z konfiguracji
	QString p7z_path = settings->value("qnapi/7z_path", "").toString();
	if(!p7z_path.isEmpty() && QFileInfo(p7z_path).isExecutable())
		return p7z_path;

	// Przeszukiwanie sciezek systemowych
	QString path = QProcess::systemEnvironment().filter(QRegExp("^PATH=(.*)$")).value(0);
	QStringList paths = path.mid(5).split(":");
	paths.removeAll("");

	if(paths.size() == 0)
		paths << "/usr/bin" << "/usr/local/bin";

	QStringList binaries;
	binaries << "7z" << "7za" << "7zr";

	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		for(QStringList::iterator j = binaries.begin(); j != binaries.end(); j++)
		{
			p7z_path = (*i) + "/" + (*j);
			if(QFileInfo(p7z_path).isExecutable())
				return p7z_path;
		}
	}

#ifdef Q_WS_MAC
// Pakiet binarny dla OS X, ktory przygotowal adrian5632 zawiera
// program 7z w odpowiednim katalogu
	p7z_path = QFileInfo(QApplication::applicationDirPath() + "/../Resources/7z").absoluteFilePath();
	if(QFileInfo(p7z_path).isExecutable())
		return p7z_path;
#endif

#ifdef Q_WS_WIN
// Pod systemem Windows program 7za.exe musi byc w tym samym katalogu,
// co qnapi.exe
	p7z_path = QFileInfo(QApplication::applicationDirPath() + "/7za.exe").absoluteFilePath();
	if(QFileInfo(p7z_path).isExecutable())
		return p7z_path;
#endif

	// Jesli wszystko inne zawiodlo...;)
	return "7z";
}

void QNapiConfig::setP7zipPath(const QString & path)
{
	settings->setValue("qnapi/7z_path", path);
}

QString QNapiConfig::tmpPath()
{
	return settings->value("qnapi/tmp_path", QDir::tempPath()).toString();
}

void QNapiConfig::setTmpPath(const QString & path)
{
	settings->setValue("qnapi/tmp_path", path);
}

QString QNapiConfig::nick()
{
	return settings->value("qnapi/nick", "").toString();
}

void QNapiConfig::setNick(const QString & nick)
{
	settings->setValue("qnapi/nick", nick);
}

QString QNapiConfig::pass()
{
	return settings->value("qnapi/pass", "").toString();
}

void QNapiConfig::setPass(const QString & pass)
{
	settings->setValue("qnapi/pass", pass);
}

QString QNapiConfig::language()
{
	return settings->value("qnapi/language", "PL").toString();
}

void QNapiConfig::setLanguage(const QString & language)
{
	settings->setValue("qnapi/language", language);
}

bool QNapiConfig::noBackup()
{
	return settings->value("qnapi/no_backup", false).toBool();
}

void QNapiConfig::setNoBackup(bool noBackup)
{
	settings->setValue("qnapi/no_backup", noBackup);
}


bool QNapiConfig::useBrushedMetal()
{
	return settings->value("qnapi/use_brushed_metal", true).toBool();
}

void QNapiConfig::setUseBrushedMetal(bool use)
{
	settings->setValue("qnapi/use_brushed_metal", use);
}

bool QNapiConfig::ppEnabled()
{
	return settings->value("qnapi/post_processing", false).toBool();
}

void QNapiConfig::setPpEnabled(bool enable)
{
	settings->setValue("qnapi/post_processing", enable);
}

bool QNapiConfig::ppChangeEncoding()
{
	return settings->value("qnapi/change_encoding", false).toBool();
}

void QNapiConfig::setPpChangeEncoding(bool change)
{
	settings->setValue("qnapi/change_encoding", change);
}

bool QNapiConfig::ppAutoDetectEncoding()
{
	return settings->value("qnapi/auto_detect_encoding", false).toBool();
}

void QNapiConfig::setPpAutoDetectEncoding(bool change)
{
	settings->setValue("qnapi/auto_detect_encoding", change);
}

QString QNapiConfig::ppEncodingFrom()
{
	return settings->value("qnapi/enc_from", "windows-1250").toString();
}

void QNapiConfig::setPpEncodingFrom(const QString & encoding)
{
	settings->setValue("qnapi/enc_from", encoding);
}

QString QNapiConfig::ppEncodingTo()
{
	return settings->value("qnapi/enc_to", "UTF-8").toString();
}

void QNapiConfig::setPpEncodingTo(const QString & encoding)
{
	settings->setValue("qnapi/enc_to", encoding);
}

bool QNapiConfig::ppShowAllEncodings()
{
	return settings->value("qnapi/show_all_encodings", false).toBool();
}

void QNapiConfig::setPpShowAllEncodings(bool show)
{
	settings->setValue("qnapi/show_all_encodings", show);
}

bool QNapiConfig::ppRemoveLines()
{
	return settings->value("qnapi/remove_lines", false).toBool();
}

void QNapiConfig::setPpRemoveLines(bool remove)
{
	settings->setValue("qnapi/remove_lines", remove);
}

QStringList QNapiConfig::ppRemoveWords()
{
	QStringList defaultRemoveWords;
	defaultRemoveWords << "movie info" << "synchro ";
						
	return settings->value("qnapi/remove_words", defaultRemoveWords).toStringList();
}

void QNapiConfig::setPpRemoveWords(const QStringList & words)
{
	settings->setValue("qnapi/remove_words", words);
}

bool QNapiConfig::ppChangePermissions()
{
	return settings->value("qnapi/change_permissions", false).toBool();
}

void QNapiConfig::setPpChangePermissions(bool change)
{
	settings->setValue("qnapi/change_permissions", change);
}

QString QNapiConfig::ppPermissions()
{
	return settings->value("qnapi/permissions", "644").toString();
}

void QNapiConfig::setPpPermissions(const QString & permissions)
{
	settings->setValue("qnapi/permissions", permissions);
}

QString QNapiConfig::previousDialogPath()
{
	QString path = settings->value("qnapi/prev_dialog_path", "").toString();
	return (QDir(path).exists() ? path : QDir::homePath());
}

void QNapiConfig::setPreviousDialogPath(const QString & path)
{
	settings->setValue("qnapi/prev_dialog_path", path);
}

QStringList QNapiConfig::scanFilters()
{
	QStringList defaultScanFilters;
	defaultScanFilters << "*.avi *.asf *.divx *.dat *.mkv *.mov *.mp4 *.mpeg"
							" *.mpg *.ogm *.rm *.rmvb *.wmv" << "*.*";
						
	return settings->value("scan/filters", defaultScanFilters).toStringList();
}

void QNapiConfig::setScanFilters(const QStringList & filters)
{
	settings->setValue("scan/filters", filters);
}

QString QNapiConfig::scanSkipFilters()
{
	return settings->value("scan/skip_filters", "PL dubbing").toString();
}

void QNapiConfig::setScanSkipFilters(const QString & filters)
{
	settings->setValue("scan/skip_filters", filters);
}

bool QNapiConfig::scanSkipIfSubtitlesExists()
{
	return settings->value("scan/skip_if_subtitles_exists", false).toBool();
}

void QNapiConfig::setScanSkipIfSubtitlesExists(bool skip)
{
	settings->setValue("scan/skip_if_subtitles_exists", skip);
}

QString QNapiConfig::lastScanDir()
{
	return settings->value("scan/last_scan_dir", "").toString();
}

void QNapiConfig::setLastScanDir(const QString & dir)
{
	settings->setValue("scan/last_scan_dir", dir);
}


QNapiConfig & GlobalConfig()
{
	static QNapiConfig cfg;
	return cfg;
}

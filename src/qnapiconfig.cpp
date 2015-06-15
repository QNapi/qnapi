/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

#ifdef Q_WS_MAC
	// Pod MacOS X 7zip jest w zasobach aplikacji
	paths << QDir(QApplication::applicationDirPath() + "/../Resources").canonicalPath();
#endif

	QStringList binaries;
	binaries << "7z" << "7za" << "7zr" << "p7zip";

	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		for(QStringList::iterator j = binaries.begin(); j != binaries.end(); j++)
		{
			p7z_path = (*i) + "/" + (*j);
			if(QFileInfo(p7z_path).isExecutable())
				return p7z_path;
		}
	}


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

QString QNapiConfig::nick(const QString & engine)
{
	QString nick = settings->value(engine + "/nick", "").toString();

	// Konwersja z konfigow <= 0.1.5
	if(nick.isEmpty() && (engine == "NapiProjekt"))
	{
		nick = settings->value("qnapi/nick", "").toString();

		if(!nick.isEmpty())
		{
			settings->remove("qnapi/nick");
			setNick("NapiProjekt", nick);
		}
	}

	return nick;
}

void QNapiConfig::setNick(const QString & engine, const QString & nick)
{
	settings->setValue(engine + "/nick", nick);
}

QString QNapiConfig::pass(const QString & engine)
{
	QString pass = settings->value(engine + "/pass", "").toString();

	if(pass.isEmpty() && (engine == "NapiProjekt"))
	{
		pass = settings->value("qnapi/pass", "").toString();

		if(!pass.isEmpty())
		{
			settings->remove("qnapi/pass");
			setPass("NapiProjekt", pass);
		}
	}

	return pass;
}

void QNapiConfig::setPass(const QString & engine, const QString & pass)
{
	settings->setValue(engine + "/pass", pass);
}

QString QNapiConfig::language()
{
	return settings->value("qnapi/language", "pl").toString();
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

#ifdef Q_OS_MAC
bool QNapiConfig::showDockIcon()
{
	const bool show_default = true;
	
	QString infoPlistPath = QFileInfo(QApplication::applicationDirPath() + "/../Info.plist").canonicalFilePath();

	QFile plistFile(infoPlistPath);
	
	QDomDocument doc;
	if(!doc.setContent(&plistFile))
		return show_default;

	if(!doc.hasChildNodes())
		return show_default;

	QDomNodeList nodes = doc.childNodes();
	
	QDomNode node;
	int i;
	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);
		if(node.nodeName() == "plist")
			break;
	}
	
	if(i == nodes.size())
		return show_default;

	if(!node.hasChildNodes())
		return show_default;

	nodes = node.childNodes();
	
	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);
		if(node.nodeName() == "dict")
			break;
	}

	if(i == nodes.size())
		return show_default;
	
	if(!node.hasChildNodes())
		return show_default;
	
	nodes = node.childNodes();

	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);

		QString subText;

		if(node.hasChildNodes())
		{
			subText = node.childNodes().at(0).toText().data();
		}

		if(subText == "LSUIElement")
			break;
	}

	if(i < nodes.size())
	{
		node = node.nextSibling();
		return (node.nodeName() != "true");
	}

	return show_default;
	
}

void QNapiConfig::setShowDockIcon(bool show)
{
	QString infoPlistPath = QFileInfo(QApplication::applicationDirPath() + "/../Info.plist").canonicalFilePath();
	
	QFile plistFile(infoPlistPath);
	
	QDomDocument doc;
	if(!doc.setContent(&plistFile) || !doc.hasChildNodes())
		return;
	
	QDomNodeList nodes = doc.childNodes();
	
	QDomNode node;
	int i;
	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);
		if(node.nodeName() == "plist")
			break;
	}
	
	if((i == nodes.size()) || !node.hasChildNodes())
		return;
	
	nodes = node.childNodes();
	
	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);
		if(node.nodeName() == "dict")
			break;
	}
	
	if((i == nodes.size()) || !node.hasChildNodes())
		return;
	
	nodes = node.childNodes();
	
	for(i = 0; i < nodes.size(); ++i)
	{
		node = nodes.at(i);
		
		QString subText;
		
		if(node.hasChildNodes())
		{
			subText = node.childNodes().at(0).toText().data();
		}
		
		if(subText == "LSUIElement")
			break;
	}
	
	if(i >= nodes.size())
		return;
	
	node = node.nextSibling();
	
	node.toElement().setTagName(show ? "false" : "true");

	QString modifiedContent = doc.toString(4);

	plistFile.close();
	
	if(!plistFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return;

	QTextStream plistStream(&plistFile);
	
	plistStream << modifiedContent;
	
	plistFile.close();
	
}

bool QNapiConfig::useBrushedMetal()
{
	return settings->value("qnapi/use_brushed_metal", false).toBool();
}

void QNapiConfig::setUseBrushedMetal(bool use)
{
	settings->setValue("qnapi/use_brushed_metal", use);
}

#endif

QList<QPair<QString, bool> > QNapiConfig::engines()
{
	QList<QVariant> inList = settings->value("qnapi/engines").toList();
	QList<QPair<QString, bool> > map;
    QSet<QString> names;

	foreach(QVariant v, inList)
	{
		QStringList sl = v.toStringList();
		if(sl.size() != 2) continue;

		QString key = sl.at(0);
		bool value = (sl.at(1) == "1");
		map << qMakePair(key, value);
        names << key;
	}

    QStringList allNames;
    allNames << "NapiProjekt" << "Napisy24" << "OpenSubtitles";

    QString name;
    foreach(name, allNames) {
        if(!names.contains(name)) {
          map << qMakePair(name, true);
        }
    }

	return map;
}

QStringList QNapiConfig::enginesList()
{
	QList<QPair<QString, bool> > map = engines();
	QStringList list;

	for(int i = 0; i < map.size(); ++i)
	{
		QPair<QString,bool> e = map.at(i);
		if(e.second) list << e.first;
	}

	if(list.isEmpty())
	{
        list << "NapiProjekt" << "OpenSubtitles" << "Napisy24";
	}

	return list;
}

void QNapiConfig::setEngines(QList<QPair<QString, bool> > engines)
{
	QList<QVariant> outList;
	for(int i = 0; i < engines.size(); ++i)
	{
		QPair<QString, bool> e = engines.at(i);

		QStringList sl;
		sl << e.first << (e.second ? "1" : "0");
		outList << sl;
	}

	settings->setValue("qnapi/engines", outList);
}

SearchPolicy QNapiConfig::searchPolicy()
{
	return (SearchPolicy)settings->value("qnapi/search_policy", 0).toInt();
}

void QNapiConfig::setSearchPolicy(SearchPolicy policy)
{
	settings->setValue("qnapi/search_policy", policy);
}

DownloadPolicy QNapiConfig::downloadPolicy()
{
	return (DownloadPolicy)settings->value("qnapi/download_policy", 1).toInt();
}

void QNapiConfig::setDownloadPolicy(DownloadPolicy policy)
{
	settings->setValue("qnapi/download_policy", policy);
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
	bool ok;
	int perm = settings->value("qnapi/permissions", 644).toInt(&ok);
	if(!ok || perm > 777)
		perm = 644;
	QString str = QString("%1").arg(perm);
	while(str.size() < 3)
		str = QString("0") + str;
	return str;
}

void QNapiConfig::setPpPermissions(const QString & permissions)
{
	bool ok;
	int perm = permissions.toInt(&ok);
	if(ok && perm <= 777)
	{
		QString str = QString("%1").arg(perm);
		while(str.size() < 3)
			str = QString("0") + str;
		settings->setValue("qnapi/permissions", str);
	}
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
	defaultScanFilters << "*.avi *.asf *.divx *.mkv *.mov *.mp4 *.mpeg"
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

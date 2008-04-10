/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

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

bool QNapiConfig::changeEncoding()
{
	return settings->value("qnapi/change_encoding", false).toBool();
}

void QNapiConfig::setChangeEncoding(bool change)
{
	settings->setValue("qnapi/change_encoding", change);
}

bool QNapiConfig::autoDetectEncoding()
{
	return settings->value("qnapi/auto_detect_encoding", false).toBool();
}

void QNapiConfig::setAutoDetectEncoding(bool change)
{
	settings->setValue("qnapi/auto_detect_encoding", change);
}

QString QNapiConfig::encodingFrom()
{
	return settings->value("qnapi/enc_from", "windows-1250").toString();
}

void QNapiConfig::setEncodingFrom(const QString & encoding)
{
	settings->setValue("qnapi/enc_from", encoding);
}

QString QNapiConfig::encodingTo()
{
	return settings->value("qnapi/enc_to", "UTF-8").toString();
}

void QNapiConfig::setEncodingTo(const QString & encoding)
{
	settings->setValue("qnapi/enc_to", encoding);
}

bool QNapiConfig::showAllEncodings()
{
	return settings->value("qnapi/show_all_encodings", false).toBool();
}

void QNapiConfig::setShowAllEncodings(bool show)
{
	settings->setValue("qnapi/show_all_encodings", show);
}

bool QNapiConfig::useBrushedMetal()
{
	return settings->value("qnapi/use_brushed_metal", true).toBool();
}

void QNapiConfig::setUseBrushedMetal(bool use)
{
	settings->setValue("qnapi/use_brushed_metal", use);
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

QList<QVariant> QNapiConfig::scanFilters()
{
	QList<QVariant> defaultScanFilters;
	defaultScanFilters << "*.avi *.asf *.divx *.dat *.mkv *.mov *.mp4 *.mpeg"
							" *.mpg *.ogm *.rm *.rmvb *.wmv" << "*.*";
						
	return settings->value("scan/filters", defaultScanFilters).toList();
}

void QNapiConfig::setScanFilters(const QList<QVariant> & filters)
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

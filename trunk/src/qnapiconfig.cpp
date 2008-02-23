/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

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
#ifdef Q_WS_MAC
	// Paczka binarna dla OS X przygotowana przez adrian5632 zawiera w sobie
	// program 7z, jednak jesli uzytkownik bedzie chcial uzyc innej sciezki,
	// to powinnismy ja uwzglednic
	// thx adrian5632

	QString value_config = settings->value("qnapi/7z_path", "/usr/bin/7z").toString();
	return QFileInfo(value_config).isExecutable()
			? value_config
			: QFileInfo(QApplication::applicationDirPath() + "/../Resources/7z").absoluteFilePath();
#else
	return settings->value("qnapi/7z_path", "/usr/bin/7z").toString();
#endif
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

QString QNapiConfig::previousDialogPath()
{
	return settings->value("qnapi/prev_dialog_path", "").toString();	
}

void QNapiConfig::setPreviousDialogPath(const QString & path)
{
	settings->setValue("qnapi/prev_dialog_path", path);
}

QNapiConfig & GlobalConfig()
{
	static QNapiConfig cfg;
	return cfg;
}

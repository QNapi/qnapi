/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __QNAPICONFIG__H__
#define __QNAPICONFIG__H__

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QRegExp>
#include "version.h"

class QNapiConfig
{
	public:
		void reload();
		void save();

		bool firstRun();
		QString version();
		QString p7zipPath();
		void setP7zipPath(const QString & path);
		QString tmpPath();
		void setTmpPath(const QString & path);
		QString nick();
		void setNick(const QString & nick);
		QString pass();
		void setPass(const QString & pass);
		QString language();
		void setLanguage(const QString & language);
		bool noBackup();
		void setNoBackup(bool noBackup);
		bool changeEncoding();
		void setChangeEncoding(bool change);
		bool autoDetectEncoding();
		void setAutoDetectEncoding(bool change);
		QString encodingFrom();
		void setEncodingFrom(const QString & encoding);
		QString encodingTo();
		void setEncodingTo(const QString & encoding);
		bool showAllEncodings();
		void setShowAllEncodings(bool show);
		QString previousDialogPath();
		void setPreviousDialogPath(const QString & path);

	private:
		QNapiConfig();
		QNapiConfig(const QNapiConfig &);
		~QNapiConfig();

		QSettings *settings;

	friend QNapiConfig& GlobalConfig();
};

QNapiConfig & GlobalConfig();

#endif

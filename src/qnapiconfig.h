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
		bool useBrushedMetal();
		void setUseBrushedMetal(bool use);
		QString previousDialogPath();
		void setPreviousDialogPath(const QString & path);
		QList<QVariant> scanFilters();
		void setScanFilters(const QList<QVariant> & filters);
		QString scanSkipFilters();
		void setScanSkipFilters(const QString & filters);
		bool scanSkipIfSubtitlesExists();
		void setScanSkipIfSubtitlesExists(bool skip);
		QString lastScanDir();
		void setLastScanDir(const QString & dir);

	private:
		QNapiConfig();
		QNapiConfig(const QNapiConfig &);
		~QNapiConfig();

		QSettings *settings;

	friend QNapiConfig& GlobalConfig();
};

QNapiConfig & GlobalConfig();

#endif

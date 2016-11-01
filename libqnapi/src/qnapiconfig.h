/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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
#include <QList>
#include <QPair>

#ifdef Q_OS_MAC
#include <QtXml>
#endif

enum SearchPolicy
{
    SP_BREAK_IF_FOUND = 0,
    SP_SEARCH_ALL = 1,
    SP_SEARCH_ALL_WITH_BACKUP_LANG = 2
};

enum DownloadPolicy
{
    DP_ALWAYS_SHOW_LIST = 0,
    DP_SHOW_LIST_IF_NEEDED = 1,
    DP_NEVER_SHOW_LIST  = 2
};

enum ChangeEncodingMethod
{
    CEM_ORIGINAL = 0,
    CEM_CHANGE = 1,
    CEM_REPLACE_DIACRITICS = 2
};

class QNapiConfig
{
    public:
        void load(QString appDirPath);
        void save();

        void performMigrations();

        bool firstRun();
        QString version();

        QString p7zipPath();
        void setP7zipPath(const QString & path);

        QString tmpPath();
        void setTmpPath(const QString & path);

        QString nick(const QString & engine);
        void setNick(const QString & engine, const QString & nick);

        QString pass(const QString & engine);
        void setPass(const QString & engine, const QString & pass);

        QString language();
        void setLanguage(const QString & language);

        QString languageBackup();
        void setLanguageBackup(const QString & language);

        bool noBackup();
        void setNoBackup(bool noBackup);

        bool quietBatch();
        void setQuietBatch(bool quietBatch);

#ifdef Q_OS_MAC
        bool showDockIcon();
        void setShowDockIcon(bool show);
#endif

        QList<QPair<QString, bool> > engines();
        QStringList enginesList();
        void setEngines(QList<QPair<QString, bool> > engines);

        SearchPolicy searchPolicy();
        void setSearchPolicy(SearchPolicy policy);
        
        DownloadPolicy downloadPolicy();
        void setDownloadPolicy(DownloadPolicy policy);

        bool ppEnabled();
        void setPpEnabled(bool enable);

        ChangeEncodingMethod ppEncodingMethod();
        void setPpEncodingMethod(ChangeEncodingMethod method);
        
        bool ppAutoDetectEncoding();
        void setPpAutoDetectEncoding(bool change);

        QString ppEncodingFrom();
        void setPpEncodingFrom(const QString & encoding);

        QString ppEncodingTo();
        void setPpEncodingTo(const QString & encoding);

        bool ppShowAllEncodings();
        void setPpShowAllEncodings(bool show);

        bool ppRemoveLines();
        void setPpRemoveLines(bool remove);

        QString ppSubFormat();
        void setPpSubFormat(const QString & subFormat);

        QString ppSubExtension();
        void setPpSubExtension(const QString & subExtension);

        bool ppSkipConvertAds();
        void setPpSkipConvertAds(bool skip);

        QStringList ppRemoveWords();
        void setPpRemoveWords(const QStringList & words);

        bool changePermissions();
        void setChangePermissions(bool change);

        QString changePermissionsTo();
        void setChangePermissionsTo(const QString & permissions);

        QString previousDialogPath();
        void setPreviousDialogPath(const QString & path);

        QStringList scanFilters();
        void setScanFilters(const QStringList & filters);

        QString scanSkipFilters();
        void setScanSkipFilters(const QString & filters);

        bool scanSkipIfSubtitlesExists();
        void setScanSkipIfSubtitlesExists(bool skip);

        QString lastScanDir();
        void setLastScanDir(const QString & dir);

        QStringList movieExtensions();
        QString movieExtensionsFilter();
        QStringList subtitleExtensions();
        QString subtitleExtensionsFilter();

    private:
        QNapiConfig(const QString & qnapiVersion);
        QNapiConfig(const QNapiConfig &);
        ~QNapiConfig();

        QSettings *settings;
        bool isPortableMode;
        const QString & qnapiVersion;

    friend QNapiConfig& GlobalConfig();
};

QNapiConfig & GlobalConfig();

#endif

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

#ifndef __QNAPIPROJEKTENGINE__H__
#define __QNAPIPROJEKTENGINE__H__

#include "qnapiabstractengine.h"

const unsigned long NAPI_10MB = 10485760;

const QString napiZipPassword = "iBlm8NTigvru0Jr0";
const QString napiDownloadUrlTpl= "http://www.napiprojekt.pl/unit_napisy/dl.php"
                "?l=%1&f=%2&t=%3&v=other&kolejka=false&nick=%4&pass=%5&napios=%6";
const QString napiCheckUserUrlTpl = "http://www.napiprojekt.pl/users_check.php?nick=%1&pswd=%2";
const QString napiUploadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/upload.php"
                "?m_length=%1&m_resolution=%2x%3&m_fps=%4&m_hash=%5&m_filesize=%6";
const QString napiUploadUrlSimpleTpl = "http://www.napiprojekt.pl/unit_napisy/upload.php"
                "?m_hash=%5&m_filesize=%6";
const QString napiReportBadUrlTpl = "http://www.napiprojekt.pl/unit_napisy/zlenapisyadd.php";

class QNapiProjektEngine : public QNapiAbstractEngine
{
public:

    QNapiProjektEngine();
    ~QNapiProjektEngine();

    // zwraca nazwe modulu
    QString engineName();
    // zwraca informacje nt. modulu
    QString engineInfo();
    // zwraca ikone silnika pobierania
    QIcon engineIcon();
    // zwraca czy silnik jest konfigurowalny
    bool isConfigurable();
    // wywoluje okienko konfiguracji
    void configure(QWidget * parent);

    QUrl registrationUrl() const {
        return QUrl("http://www.napiprojekt.pl/rejestracja");
    }

    QString checksum(QString filename = "");
    bool lookForSubtitles(QString lang);
    QList<QNapiSubtitleInfo> listSubtitles();
    bool download(QUuid id);
    bool unpack(QUuid id);
    void cleanup();

    QString name()
    {
        return QString("NapiProjekt");
    }

    static bool checkUser(const QString & nick, const QString & pass);

private:

    QString p7zipPath, nick, pass;

    QString checksum(QString filename, bool limit10M);
    Maybe<QString> downloadByLangAndChecksum(QString lang, QString checksum) const;
    QString npFDigest(const QString & input) const;
    QString npLangWrapper(QString lang) const;
    QString napiOS() const;

};

#endif

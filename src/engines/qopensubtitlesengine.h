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

#ifndef __QOPENSUBTITLESENGINE__H__
#define __QOPENSUBTITLESENGINE__H__

#include "qnapiabstractengine.h"
#include "syncxmlrpc.h"

const QString openSubtitlesXmlRpcUrl = "http://api.opensubtitles.org/xml-rpc";

class QOpenSubtitlesEngine : public QNapiAbstractEngine
{
public:

    QOpenSubtitlesEngine();
    ~QOpenSubtitlesEngine();

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
        return QUrl("http://www.opensubtitles.org/newuser");
    }


    // oblicza sume kontrolna pliku filmowego
    QString checksum(QString filename = "");
    // szuka napisow
    bool lookForSubtitles(QString lang);
    // wyniki wyszukiwania
    QList<QNapiSubtitleInfo> listSubtitles();
    // probuje pobrac napisy
    bool download(QUuid id);
    // probuje rozpakowac napisy
    bool unpack(QUuid id);
    // czysci smieci, pliki tymczasowe
    void cleanup();

private:

    QString p7zipPath, lang, subFileName;
    quint64 fileSize;

    SyncXmlRpc rpc;
    QString token;


    // sprawdza czy dana instancja klasy jest zalogowana na sewerze
    bool isLogged() { return !token.isEmpty(); }
    // loguje na serwer OpenSubtitles
    bool login();
    // wylogowuje z serwera
    void logout();

};

#endif

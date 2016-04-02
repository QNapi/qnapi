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

#ifndef __QNAPIABSTRACTENGINE__H__
#define __QNAPIABSTRACTENGINE__H__

#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QIcon>
#include <QPixmap>

#include <ctime>

#include <Maybe.h>

#include "qnapiconfig.h"
#include "qnapisubtitleinfo.h"

class QNapiAbstractEngine
{
public:

    // destruktor
    virtual ~QNapiAbstractEngine() {}

    // ustawia sciezke do pliku filmowego
    void setMoviePath(const QString & path);
    // zwraca sciezke do pliku filmowego
    QString moviePath();

    // dopasowuje napisy do pliku z filmem
    bool match();

#ifndef Q_OS_WIN
    // zmienia uprawnienia do pliku z napisami
    bool changeSubtitlesPermissions(QFile::Permissions permissions);
#endif

    // powinna zwracac nazwe modulu
    virtual QString engineName() = 0;
    // powinna zwracac informacje nt. modulu (prawa autorskie itp.)
    virtual QString engineInfo() = 0;
    // zwraca ikone silnika pobierania
    virtual QIcon engineIcon() = 0;
    // zwraca czy silnik jest konfigurowalny
    virtual bool isConfigurable() = 0;
    // wywoluje okienko konfiguracji
    virtual void configure(QWidget * parent) = 0;
    // rejestracja uzytkownika
    virtual QUrl registrationUrl() const = 0;

    // czysci wewnetrzne listy znalezionych napisow w silniku
    virtual void clearSubtitlesList();
    // powinna obliczac i zwracac sume kontrolna pliku filmowego,
    // a takze ustawiac wartosc zmiennej checksum
    virtual QString checksum(QString filename = "") = 0;
    // szuka napisow w podanym jezyku
    virtual bool lookForSubtitles(QString lang) = 0;
    // zwraca liste dostepnych napisow
    virtual QList<QNapiSubtitleInfo> listSubtitles() = 0;
    // powinna pobierac napisy do filmu i zapisywac w jakims pliku tymczasowym
    virtual bool download(QUuid id) = 0;
    // powinna rozpakowywac pobrane napisy, a ich sciezke zapisywac w polu
    // subtitlesTmpPath
    virtual bool unpack(QUuid id) = 0;
    // powinna czyscic pliki tymczasowe itd.
    virtual void cleanup() = 0;


protected:

    QList<QNapiSubtitleInfo> subtitlesList;

    // sciezka do pliku filmowego
    QString movie;
    // sciezka do napisow (zazwyczaj taka sama jak do napisow, tylko z innym rozszerzeniem)
    QString subtitles;
    // sciezka do tymczasowego pliku z napisami
    QString subtitlesTmp;
    // sciezka do katalogu tymczasowego
    QString tmpPath;
    // suma kontrolna pliku filmowego
    QString checkSum;
    // okresla czy uzytkownik wylaczyl wykonywanie kopii zapasowej, gdy napisy do
    // zadanego filmu juz istnieja
    bool noBackup;

    // konstruktor klasy
    QNapiAbstractEngine()
    {
        tmpPath = GlobalConfig().tmpPath();
        noBackup = GlobalConfig().noBackup();
    }

    Maybe<QNapiSubtitleInfo> resolveById(QUuid id);
    void updateSubtitleInfo(const QNapiSubtitleInfo & si);


    // generuje nazwe dla pliku tymczasowego
    QString generateTmpFileName() const;
    // generuje sciezke dla pliku tymczasowego w katalogu tymczasowym
    QString generateTmpPath() const;

    friend class QNapi;
};

#endif

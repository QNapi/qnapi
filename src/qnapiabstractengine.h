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

#ifndef __QNAPIABSTRACTENGINE__H__
#define __QNAPIABSTRACTENGINE__H__

#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QIcon>
#include <QPixmap>

#include <ctime>

#include "qnapiconfig.h"
#include "qnapisubtitleinfo.h"
#include "encodingutils.h"

class QNapiAbstractEngine
{
public:

    // destruktor
    virtual ~QNapiAbstractEngine() {};

    // ustawia sciezke do pliku filmowego
    void setMoviePath(const QString & path);
    // zwraca sciezke do pliku filmowego
    QString moviePath();
    // ustawia sciezke do pliku z napisami
    void setSubtitlesPath(const QString & path);
    // zwraca sciezke do pliku z napisami
    QString subtitlesPath();

    // dopasowuje napisy do pliku z filmem
    bool match();

    // dokonuje przetwarzania napisow
    void pp();

    // probuje wykrywac (polskie) kodowanie znakow w pliku tekstowym
    QString ppDetectEncoding(const QString & fileName, int testBufferSize = 10240);
    // zamienia znaki diakrytyczne na ASCII
    bool ppReplaceDiacriticsWithASCII();
    // zmienia kodowanie znakow w pobranych napisach
    bool ppChangeSubtitlesEncoding(const QString & from, const QString & to);
    // zmienia kodowanie znakow w napisach na podane, dokonujac autodetekcji kodowania zrodlowgo
    bool ppChangeSubtitlesEncoding(const QString & to);
    // usuwa linie z pliku zawierajace conajmniej jedno z podanej listy slow
    bool ppRemoveLinesContainingWords(QStringList wordList);
    // usuwa polskie znaki diakrytyczne ze stringa
    QString removePolishDiacritics(const QString & str);
#ifndef Q_OS_WIN
    // zmienia uprawnienia do pliku z napisami
    bool ppChangeSubtitlesPermissions(QFile::Permissions permissions);
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

    // powinna obliczac i zwracac sume kontrolna pliku filmowego,
    // a takze ustawiac wartosc zmiennej checkSym
    virtual QString checksum(QString filename = "") = 0;
    // szuka napisow w podanym jezyku
    virtual bool lookForSubtitles(QString lang) = 0;
    // zwraca liste dostepnych napisow
    virtual QList<QNapiSubtitleInfo> listSubtitles() = 0;
    // powinna pobierac napisy do filmu i zapisywac w jakims pliku tymczasowym
    virtual bool download(int idx) = 0;
    // powinna rozpakowywac pobrane napisy, a ich sciezke zapisywac w polu
    // subtitlesTmpPath
    virtual bool unpack() = 0;
    // powinna czyscic pliki tymczasowe itd.
    virtual void cleanup() = 0;

protected:

    // sciezka do pliku filmowego
    QString movie;
    // sciezka do napisow (zazwyczaj taka sama jak do napisow z innym rozszerzeniem)
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
    QNapiAbstractEngine(const QString & movieFile = "", const QString & subtitlesFile = "") 
        : movie(movieFile), subtitles(subtitlesFile)
    {
        tmpPath = GlobalConfig().tmpPath();
        noBackup = GlobalConfig().noBackup();
    };

    // generuje nazwe dla pliku tymczasowego
    QString generateTmpFileName();

private:
    EncodingUtils encodingUtils;

};

#endif

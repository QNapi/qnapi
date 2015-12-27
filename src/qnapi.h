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

#ifndef __QNAPI__H__
#define __QNAPI__H__


#include <QtCore>
#include "qnapiconfig.h"
#include "engines/qnapiabstractengine.h"
#include "engines/qnapiprojektengine.h"
#include "engines/qnapisy24engine.h"
#include "engines/qopensubtitlesengine.h"
#include "qnapisubtitleinfo.h"

// globalny menedzer pobierania napisow
class QNapi
{
    public:

        QNapi() {}
        ~QNapi();

        // sprawdza sciezke do 7zipa
        static bool checkP7ZipPath();
        // sprawdza katalog plikow tymczasowych
        static bool checkTmpPath();
        // sprawdza czy wlaczone jest przetwarzanie napisow
        static bool ppEnabled();

        // listuje dostępne silniki pobierania
        static QStringList enumerateEngines();
        // wlacza silnik pobierania napisow o podanej nazwie
        bool addEngine(QString engine);
        // za jedym zamachem wlacza silniki pobierania napisow o podanych nazwach
        bool addEngines(QStringList engines);

        // ustawia sciezke do pliku z filmem
        void setMoviePath(QString path);
        // zwraca sciezke do pliku z filmem
        QString moviePath();
        // sprawdza uprawnienia zapisu do katalogu docelowego
        bool checkWritePermissions();

        void clearSubtitlesList();
        // oblicza sume kontrolna pliku z filmem
        void checksum();
        // szuka napisow w podanym jezyku
        bool lookForSubtitles(QString lang, QString engine = "");
        // szuka napisow w podanych jezykach
        bool lookForSubtitles(QStringList languages, QString engine = "");
        // zwraca liste znalezionych napisow
        QList<QNapiSubtitleInfo> listSubtitles();

        // decyduje (na podst. konfiguracji) czy pokazywac liste napisow
        bool needToShowList();
        // jesli nie potrzeba pokazywac listy, zwracamy najlepszy indeks napisow
        int bestIdx();

        // pobiera napisy o i-tym indeksie z listy subtitlesList
        bool download(int i);
        // rozpakowuje pobrane napisy, a sciezke do nich zapisuje w subtitlesTmp
        bool unpack(int i);
        // dopasowuje pobrane napisy
        bool match();
        // wykonuje przetwarzanie na dopasowanych napisach
        void pp();

        // czysci rozne smieci i pliki tymczasowe
        void cleanup();
        // zwraca komunikat o bledzie, w przypadku niepowodzenia
        QString error();

        // zwraca wskaznik do zaladowanego! silnika z napisami po nazwie
        QNapiAbstractEngine * engineByName(QString name);
        // na odwrot ;)
        QString nameByEngine(QNapiAbstractEngine * engine);

        // listuje zaladowane moduly pobierania
        QStringList listLoadedEngines();

    private:

        // sciezka do pliku z filmem
        QString movie;
        // aktualny komunikat o bledzie
        QString errorMsg;
        // lista zaladowanych silnikow z napisami
        QList<QNapiAbstractEngine*> enginesList;
        // lista znalezionych napisow
        QList<QNapiSubtitleInfo> subtitlesList;
        // wskaznik do obiektu silnika, na ktorym aktualnie pracujemy
        // ustawiany po wykonaniu metody download()
        QNapiAbstractEngine * currentEngine;
        
        // najlepszy indeks napisow
        int theBestIdx;

};


#endif // __QNAPI__H__

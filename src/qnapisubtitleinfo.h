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

#ifndef __QNAPISUBTITLEINFO__H__
#define __QNAPISUBTITLEINFO__H__

#include <QUuid>

// Orientacyjne okreslenie czy napisy na pewno pasuja lub nie do naszego filmu
enum QNapiSubtitleResolution
{
    // brak napisów
    SUBTITLE_NONE,
    // podejrzenie, ze napisy nie pasuja
    // (np. zostaly oznaczone jako nieprawidlowe)
    SUBTITLE_BAD,
    // nie wiadomo (lub nie jestesmy pewni)
    SUBTITLE_UNKNOWN,
    // napisy prawdopodobnie pasuja (np. nazwa pliku czy releasu sie zgadza)
    SUBTITLE_GOOD
};

// struktura opisujaca napisy
struct QNapiSubtitleInfo
{
    QNapiSubtitleInfo(QString _lang = "",
                      QString _engine = "",
                      QString _sourceLocation = "",
                      QString _name = "",
                      QString _comment = "",
                      QString _format = "",
                      QNapiSubtitleResolution _resolution = SUBTITLE_UNKNOWN)
        : lang(_lang.toLower()),
          engine(_engine),
          sourceLocation(_sourceLocation),
          name(_name),
          comment(_comment),
          format(_format),
          resolution(_resolution),
          id(QUuid::createUuid()) {}

    QString lang;
    QString engine;
    QString sourceLocation;
    QString name;
    QString comment;
    QString format;
    QNapiSubtitleResolution resolution;
    QUuid id;

    bool operator<(const QNapiSubtitleInfo& other) const {
        return resolution > other.resolution;
    }

    static QNapiSubtitleInfo fromFailed(QString name) {
        return QNapiSubtitleInfo("", "", "",name, "" ,"", SUBTITLE_NONE);
    }
};

Q_DECLARE_METATYPE(QNapiSubtitleInfo);

typedef QList<QNapiSubtitleInfo> QNapiSubtitleInfoList;

#endif

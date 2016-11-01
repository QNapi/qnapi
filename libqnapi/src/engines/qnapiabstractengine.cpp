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

#include "qnapiabstractengine.h"
#include "subconvert/subtitleformatsregistry.h"
#include <QFlags>
#include <QDir>

// ustawia sciezke do pliku filmowego
void QNapiAbstractEngine::setMoviePath(const QString & path)
{
    movie = path;
}

// zwraca sciezke do pliku filmowego
QString QNapiAbstractEngine::moviePath()
{
    return movie;
}

void QNapiAbstractEngine::clearSubtitlesList()
{
    foreach(QNapiSubtitleInfo s, subtitlesList)
    {
        if(QFile::exists(s.sourceLocation))
            QFile::remove(s.sourceLocation);
    }
    subtitlesList.clear();
}

Maybe<QNapiSubtitleInfo> QNapiAbstractEngine::resolveById(QUuid id)
{
    foreach(QNapiSubtitleInfo s, subtitlesList)
    {
        if(s.id == id)
            return just(s);
    }
    return nothing();
}

void QNapiAbstractEngine::updateSubtitleInfo(const QNapiSubtitleInfo & si)
{
    for(int i = 0; i < subtitlesList.size(); ++i)
    {
        if(subtitlesList[i].id == si.id)
        {
            subtitlesList[i] = si;
            break;
        }
    }

}

// generuje nazwe dla pliku tymczasowego
QString QNapiAbstractEngine::generateTmpFileName() const
{
    static bool gen_inited;
    if(!gen_inited)
    {
        qsrand(time(0));
        gen_inited = true;
    }
    return QString("QNapi.%1.tmp").arg(qrand());
}

QString QNapiAbstractEngine::generateTmpPath() const
{
    QString newTmpFilePath = QString("%1/%2")
            .arg(tmpPath)
            .arg(generateTmpFileName());
    return QDir::toNativeSeparators(newTmpFilePath);
}


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


// dopasowuje napisy do pliku z filmem
bool QNapiAbstractEngine::match()
{
    QFileInfo stf(subtitlesTmp);

    if(!stf.exists())
        return false;

    QString targetExtension = stf.suffix();

    if(GlobalConfig().ppEnabled())
    {
        if(!GlobalConfig().ppSubFormat().isEmpty() && GlobalConfig().ppSubExtension().isEmpty())
        {
            targetExtension = GlobalFormatsRegistry().select(GlobalConfig().ppSubFormat())->defaultExtension();
        }
        else if(!GlobalConfig().ppSubExtension().isEmpty())
        {
            targetExtension = GlobalConfig().ppSubExtension();
        }
    }

    QFileInfo mf(movie);

    subtitles = mf.path() + "/" + mf.completeBaseName() + "." + targetExtension;

    QFileInfo sf(subtitles);

    if(!QFileInfo(sf.absolutePath()).isWritable())
        return false;

    if(QFile::exists(subtitles))
    {
        if(!noBackup)
        {
            QString newName = mf.path() + QDir::separator() + mf.completeBaseName() + "_kopia." + sf.suffix();

            if(QFile::exists(newName))
                QFile::remove(newName);

            QFile::rename(subtitles, newName);          
        }
        else
            QFile::remove(subtitles);
    }

    bool r = false;

#ifdef Q_OS_WIN
    // Pod windowsem, aby "wyczyscic" atrybuty pliku, tworzymy go na nowo
    QFile f(subtitles), f2(subtitlesTmp);
    if(!f.open(QIODevice::WriteOnly) || !f2.open(QIODevice::ReadOnly))
    {
        f.close();
    }
    else
    {
        r = f.write(f2.readAll()) > 0;
        f2.close();
        f.close();
    }
#else
    // pod normalnymi OS-ami nie trzeba sie gimnastykowac z atrybutami
    r = QFile::copy(subtitlesTmp, subtitles);

    // Zmiana uprawnien do pliku
    if(GlobalConfig().changePermissions())
    {
        bool validPermissions;
        int permInt = GlobalConfig().changePermissionsTo().toInt(&validPermissions, 8);

        if(validPermissions)
        {
            int perm = 0;
            perm |= (permInt & 0700) << 2;
            perm |= (permInt & 0070) << 1;
            perm |= (permInt & 0007);
            changeSubtitlesPermissions(QFile::Permissions(perm));
        }
    }
#endif

    return r;
}

#ifndef Q_OS_WIN
// Zmienia uprawnienia do pliku z napisami
bool QNapiAbstractEngine::changeSubtitlesPermissions(QFile::Permissions permissions)
{
    if(!QFileInfo(subtitles).exists())
        return false;

    return QFile::setPermissions(subtitles, permissions);
}
#endif


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
QString QNapiAbstractEngine::generateTmpFileName()
{
    static bool gen_inited;
    if(!gen_inited)
    {
        qsrand(time(0));
        gen_inited = true;
    }
    return QString("QNapi.%1.tmp").arg(qrand());
}

QString QNapiAbstractEngine::generateTmpPath()
{
    QString newTmpFilePath = QString("%1/%2")
            .arg(tmpPath)
            .arg(generateTmpFileName());
    return QDir::toNativeSeparators(newTmpFilePath);
}


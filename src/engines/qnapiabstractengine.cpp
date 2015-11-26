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
#include "subconvert/subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"

#include <QFlags>
#include <QDebug>

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

// ustawia sciezke do pliku z napisami
void QNapiAbstractEngine::setSubtitlesPath(const QString & path)
{
    subtitles = path;
}

// zwraca sciezke do pliku z napisami
QString QNapiAbstractEngine::subtitlesPath()
{
    return subtitles;
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

// Dokonuje post-przetwarzania pliku z napisami (na podstawie konfiguracji)
void QNapiAbstractEngine::pp()
{
    // Usuwanie linii z plikow z napisami
    if(GlobalConfig().ppRemoveLines())
    {
        ppRemoveLinesContainingWords(GlobalConfig().ppRemoveWords());
    }

    // Zmiana kodowania pobranych napisow
    switch (GlobalConfig().ppEncodingMethod()) {
        case CEM_REPLACE_DIACRITICS:
            ppReplaceDiacriticsWithASCII();
        break;
        case CEM_CHANGE:
            // Jesli automatycznie nie uda mu sie wykryc kodowania, to jako kodowania
            // zrodlowego uzywa kodowania wybranego przez uzytkownika
            if (!GlobalConfig().ppAutoDetectEncoding()
                || !ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingTo()))
            {
                ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingFrom(),
                                            GlobalConfig().ppEncodingTo());
            }
        break;
        case CEM_ORIGINAL:
            // Nie ruszaj pobranych napisów!
        break;
    }

    if(!GlobalConfig().ppSubFormat().isEmpty())
    {
        SubtitleConverter sc;
        QString targetFormat = GlobalConfig().ppSubFormat();
        sc.convertSubtitles(subtitlesTmp, targetFormat, subtitlesTmp, movie);
    }
}

// zamienia znaki diakrytyczne na ASCII
bool QNapiAbstractEngine::ppReplaceDiacriticsWithASCII()
{
    if(!QFileInfo(subtitlesTmp).exists())
        return false;

    QString from = encodingUtils.detectFileEncoding(subtitlesTmp);

    if(from.isEmpty())
        return false;

    QFile f(subtitlesTmp);
    if(!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray fileContent = f.readAll();

    QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
    f.close();

    fileContent.clear();
    fileContent.append(encodingUtils.replaceDiacriticsWithASCII(contentStr));

    if(!f.open(QIODevice::WriteOnly))
        return false;

    f.write(fileContent);
    f.close();

    return true;
}


// Konwertuje napisy z jednego kodowania na inne
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & from, const QString & to)
{
    QFile f(subtitlesTmp);
    if(!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray fileContent = f.readAll();
    
    QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
    f.close();

    if(to.compare("UTF-8", Qt::CaseInsensitive) != 0)
    {
        fileContent = QTextCodec::codecForName(qPrintable(to))
                        ->fromUnicode(contentStr.constData(), contentStr.size());
    }
    else
    {
        fileContent.clear();
        fileContent.append(contentStr);
    }

    if(!f.open(QIODevice::WriteOnly))
        return false;

    f.write(fileContent);
    f.close();

    return true;
}

// Konwertuje napisy z jednego kodowania na inne, dokonujac proby autodetekcji
// kodowania zrodlowego
bool QNapiAbstractEngine::ppChangeSubtitlesEncoding(const QString & to)
{
    if(!QFileInfo(subtitlesTmp).exists())
        return false;

    QString from = encodingUtils.detectFileEncoding(subtitlesTmp);

    if(from.isEmpty())
        return false;

    return ppChangeSubtitlesEncoding(from, to);
}

// Usuwanie linii zawierajacych podane slowa z pliku z napisami
bool QNapiAbstractEngine::ppRemoveLinesContainingWords(QStringList wordList)
{
    if(!QFileInfo(subtitlesTmp).exists())
        return false;

    wordList = wordList.filter(QRegExp("^(.+)$"));

    QString fromCodec = encodingUtils.detectFileEncoding(subtitlesTmp);

    if(fromCodec.isEmpty())
        fromCodec = GlobalConfig().ppEncodingFrom();

    QFile f(subtitlesTmp);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QList<QByteArray> lines = f.readAll().split('\n');
    QList<QByteArray> out;

    foreach(QByteArray line, lines)
    {
        int i;
        while((i = line.indexOf('\r')) >= 0)
            line.remove(i, 1);

        QTextStream ts(line);
        ts.setCodec(qPrintable(fromCodec));
        QString encLine = ts.readAll();

        if(encLine.isEmpty())
        {
            out << line;
            continue;
        }

        bool found = false;
        foreach(QString word, wordList)
        {
            if(encLine.contains(word, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }
        if(found) continue;

        out << line;
    }

    f.close();

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    foreach(QByteArray line, out)
    {
        f.write(line);
        f.write("\n", 1);
    }
    f.close();

    return true;
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

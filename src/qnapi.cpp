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

#include "qnapi.h"
#include "qnapiconfig.h"
#include "qsubpostprocess.h"
#include "engines/qnapiprojektengine.h"
#include "engines/qnapisy24engine.h"
#include "engines/qopensubtitlesengine.h"

#include <QtAlgorithms>

QNapi::~QNapi()
{
    cleanup();

    foreach(QNapiAbstractEngine *e, enginesList)
    {
        if(e) delete e;
    }
}

bool QNapi::checkP7ZipPath()
{
    return QFileInfo(GlobalConfig().p7zipPath()).isExecutable();
}

bool QNapi::checkTmpPath()
{
    QFileInfo f(GlobalConfig().tmpPath());
    return f.isDir() && f.isWritable();
}

bool QNapi::ppEnabled()
{
    return GlobalConfig().ppEnabled();
}

QStringList QNapi::enumerateEngines()
{
    QStringList engines;
    engines << "NapiProjekt";
    engines << "OpenSubtitles";
    engines << "Napisy24";
    return engines;
}

bool QNapi::addEngine(QString engine)
{
    if(engine == "NapiProjekt")
    {
        enginesList << (new QNapiProjektEngine());
        return true;
    }
    else if(engine == "OpenSubtitles")
    {
        enginesList << (new QOpenSubtitlesEngine());
        return true;
    }
    else if(engine == "Napisy24")
    {
        enginesList << (new QNapisy24Engine());
        return true;
    }
    else
    {
        errorMsg = QString("Nieobsługiwany silnik pobierania: %1.").arg(engine);
        return false;
    }
}

bool QNapi::addEngines(QStringList engines)
{
    foreach(QString e, engines)
    {
        if(!addEngine(e))
            return false;
    }
    return true;
}

void QNapi::setMoviePath(QString path)
{
    movie = path;
    currentEngine = 0;
}

QString QNapi::moviePath()
{
    return movie;
}

bool QNapi::checkWritePermissions()
{
    return QFileInfo(QFileInfo(movie).path()).isWritable();
}

void QNapi::clearSubtitlesList()
{
    subtitlesList.clear();
    foreach(QNapiAbstractEngine *e, enginesList)
    {
        e->clearSubtitlesList();
    }
}

void QNapi::checksum()
{
    foreach(QNapiAbstractEngine *e, enginesList)
    {
        e->checksum(movie);
    }
}

bool QNapi::lookForSubtitles(QString lang, QString engine)
{
    bool result = false;


    if(engine.isEmpty())
    {
        foreach(QNapiAbstractEngine *e, enginesList)
        {
            e->setMoviePath(movie);
            result = e->lookForSubtitles(lang) || result;
        }
    }
    else
    {
        QNapiAbstractEngine *e = engineByName(engine);
        if(e)
        {
            e->setMoviePath(movie);
            result = e->lookForSubtitles(lang);
        }
    }

    if(!result)
    {
        errorMsg = "Nie znaleziono napisów!";
    }

    return result;
}

QList<QNapiSubtitleInfo> QNapi::listSubtitles()
{
    subtitlesList.clear();

    foreach(QNapiAbstractEngine *e, enginesList)
    {
        QList<QNapiSubtitleInfo> list =  e->listSubtitles();
        subtitlesList << list;
    }

    QString mainLang = GlobalConfig().language();
    QString backupLang = GlobalConfig().languageBackup();

    auto langRank = [&](QString lang) {
        if(lang == mainLang) return 0;
        if(lang == backupLang) return 1;
        return 2;
    };

    qStableSort(subtitlesList.begin(), subtitlesList.end(),
                [&](const QNapiSubtitleInfo & si1, const QNapiSubtitleInfo & si2) {
       return langRank(si1.lang) < langRank(si2.lang);
    });

    return subtitlesList;
}

bool QNapi::needToShowList()
{
    theBestIdx = 0;

    int i = 0;
    bool foundBestIdx = false;
    foreach(QNapiSubtitleInfo s, listSubtitles())
    {
        if(s.resolution == SUBTITLE_GOOD)
        {
            theBestIdx = i;
            foundBestIdx = true;
            break;
        }
        ++i;
    }

    if(GlobalConfig().downloadPolicy() == DP_ALWAYS_SHOW_LIST)  
        return true;
    if(GlobalConfig().downloadPolicy() == DP_NEVER_SHOW_LIST)   
        return false;

    if(listSubtitles().size() <= 1)
        return false;

    return !foundBestIdx;
}

int QNapi::bestIdx()
{
    return theBestIdx;
}

bool QNapi::download(int i)
{
    QNapiSubtitleInfo s = subtitlesList[i];
    currentEngine = engineByName(s.engine);
    if(!currentEngine) return false;
    return currentEngine->download(s.id);
}

bool QNapi::unpack(int i)
{
    return currentEngine
            ? currentEngine->unpack(subtitlesList[i].id)
            : false;
}

bool QNapi::match()
{
    return currentEngine
            ? currentEngine->match()
            : false;
}

void QNapi::pp()
{
    if(currentEngine) {
        QSubPostProcess pp(currentEngine->movie,
                           currentEngine->subtitlesTmp);
        pp.perform();
    }
}

void QNapi::cleanup()
{
    foreach(QNapiAbstractEngine *e, enginesList)
    {
        e->cleanup();
    }
}

QString QNapi::error()
{
    return errorMsg;
}

QNapiAbstractEngine * QNapi::engineByName(QString name)
{
    foreach(QNapiAbstractEngine *e, enginesList)
    {
        if(name == (e->engineName()))
        {
            return e;
        }
    }

    return 0;
}

QString QNapi::nameByEngine(QNapiAbstractEngine * engine)
{
    return engine->engineName();
}

QStringList QNapi::listLoadedEngines()
{
    QStringList list;
    foreach(QNapiAbstractEngine *e, enginesList)
    {
        list << e->engineName();
    }
    return list;
}

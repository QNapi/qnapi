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

#include "subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"
#include "qnapiconfig.h"
#include "libmediainfomovieinfoparser.h"
#include "movieinfo.h"
#include "version.h"
#include <cmath>
#include <QTextStream>
#include "encodingutils.h"
#include <QDebug>

QString SubtitleConverter::detectFormat(const QString &subtitleFile)
{
    const QStringList & lines = readFile(subtitleFile, "UTF-8", 15);
    return detectFormat(lines);
}

QString SubtitleConverter::detectFormat(const QStringList & subtitleLines)
{
    foreach(QString format, GlobalFormatsRegistry().enumerateFormats())
    {
        if(GlobalFormatsRegistry().select(format)->detect(subtitleLines))
        {
            return format;
        }
    }
    return "";
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetFileName,
                                         double movieFPS,
                                         double fpsRatio,
                                         double delayOffset)
{
    return convertSubtitles(subtitleFile, targetFormatName, targetFileName, [=]() -> double { return movieFPS; }, fpsRatio, delayOffset);
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetFileName,
                                         QString movieFile)
{
    return convertSubtitles(subtitleFile, targetFormatName, targetFileName, [&]() -> double {
        LibmediainfoMovieInfoParser mip;
        MovieInfo mfm = mip.parseFile(movieFile);
        if(mfm.isFilled)
            return mfm.frameRate;
        else
            return 0.0;
    });
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetFileName,
                                         std::function<double ()> determineFPS,
                                         double fpsRatio,
                                         double delayOffset)
{
    EncodingUtils eu;
    QString encoding = eu.detectFileEncoding(subtitleFile);

    QStringList subtitleLines = readFile(subtitleFile, encoding);

    QString detectedFormat = detectFormat(subtitleLines);
    if(detectedFormat.isEmpty())
        return false;

    SubtitleFormat * inputFormat = GlobalFormatsRegistry().select(detectedFormat);
    SubtitleFormat * targetFormat = GlobalFormatsRegistry().select(targetFormatName);


    SubFile sf = inputFormat->decode(subtitleLines);
    double frameRate = 0.0;

    if(inputFormat->isTimeBased() != targetFormat->isTimeBased())
    {
        frameRate = determineFPS();
        if(frameRate <= 0.0)
            return false;

        if(targetFormat->isTimeBased())
        {
            // convert frames to timestamps
            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = frame2ts(sf.entries[i].frameStart, frameRate);
                sf.entries[i].frameStop = frame2ts(sf.entries[i].frameStop, frameRate);
            }
        }
        else
        {
            // convert timestamps to frames
            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = ts2frame(sf.entries[i].frameStart, frameRate);
                sf.entries[i].frameStop = ts2frame(sf.entries[i].frameStop, frameRate);
            }
        }
    }

    if(fpsRatio != 1.0)
    {
        for(int i = 0; i < sf.entries.size(); ++i)
        {
            sf.entries[i].frameStart = (long)floor(fpsRatio * sf.entries[i].frameStart);
            sf.entries[i].frameStop = (long)floor(fpsRatio * sf.entries[i].frameStop);
        }
    }

    if(delayOffset != 0.0)
    {
        long offset = (long)(delayOffset * 1000.0);

        if(targetFormat->isTimeBased())
        {
            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = std::max(0L, sf.entries[i].frameStart + offset);
                sf.entries[i].frameStop = std::max(0L, sf.entries[i].frameStop + offset);
            }
        } else {
            if(frameRate <= 0.0)
                frameRate = determineFPS();
            if(frameRate <= 0.0)
                return false;

            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = std::max(0L, ts2frame(frame2ts(sf.entries[i].frameStart, frameRate) + offset, frameRate));
                sf.entries[i].frameStop = std::max(0L, ts2frame(frame2ts(sf.entries[i].frameStop, frameRate) + offset, frameRate));
            }
        }
    }

    if(!GlobalConfig().ppSkipConvertAds() && !sf.entries.isEmpty())
    {
        SubToken stQNapi;
        stQNapi.type = STT_WORD;
        stQNapi.payload = "QNapi";

        if(!sf.entries.last().tokens.contains(stQNapi))
        {
            SubEntry adEntry;
            if(targetFormat->isTimeBased())
            {
                adEntry.frameStart = sf.entries.back().frameStop + 2000L;
                adEntry.frameStop = adEntry.frameStart + 8000L;
            }
            else
            {
                adEntry.frameStart = sf.entries.back().frameStop + 50L;
                adEntry.frameStop = adEntry.frameStart + 200L;
            }
            QString ad = QString("Napisy pobrane i przetworzone programem QNapi|");
            adEntry.tokens = targetFormat->decodeTokenStream(ad);
            SubToken urlToken;
            urlToken.type = STT_WORD;
            urlToken.payload = QNAPI_URL;
            adEntry.tokens.push_back(urlToken);
            sf.entries.push_back(adEntry);
        }
    }

    QStringList targetLines = targetFormat->encode(sf);

    return writeFile(targetFileName, encoding, targetLines);
}

long SubtitleConverter::ts2frame(long ts, double frameRate)
{
    return (long)floor(frameRate * ts / 1000.0);
}

long SubtitleConverter::frame2ts(long frame, double frameRate)
{
    return (long)floor(1000.0 * frame / frameRate);
}

QStringList SubtitleConverter::readFile(const QString & filename, QString encoding, long atMostLines)
{
    QStringList buff;
    long current = 0;
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&inputFile);
        in.setCodec(qPrintable(encoding));
        while (!in.atEnd() && (atMostLines == 0 || current < atMostLines))
        {
            buff += in.readLine();
            ++current;
        }
        inputFile.close();
    }
    return buff;
}

bool SubtitleConverter::writeFile(const QString & filename, QString encoding, const QStringList & lines)
{
    QFile outputFile(filename);
    if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&outputFile);
        out.setCodec(qPrintable(encoding));
        foreach(QString line, lines)
        {
            out << line << "\n";
        }
        outputFile.close();
        return true;
    } else {
        return false;
    }
}

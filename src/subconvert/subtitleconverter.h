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

#ifndef SUBTITLECONVERTER_H
#define SUBTITLECONVERTER_H

#include <QString>
#include <QStringList>
#include <functional>

class SubtitleConverter
{
public:
    QString detectFormat(const QString &subtitleFile);
    QString detectFormat(const QStringList &subtitleLines);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetFileName,
                          double movieFPS,
                          double fpsRatio,
                          double delayOffset);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetFileName,
                          QString movieFile);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetFileName,
                          std::function<double ()> determineFPS,
                          double fpsRatio = 1.0,
                          double delayOffset = 0.0);

private:
    long ts2frame(long ts, double frameRate);
    long frame2ts(long frame, double frameRate);

    QStringList readFile(const QString & filename, long atMostLines = 0);
    bool writeFile(const QString & filename, const QStringList & lines);

};

#endif // SUBTITLECONVERTER_H

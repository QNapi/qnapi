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

#include "ffprobemovieinfoparser.h"

#include <QProcess>

FFProbeMovieInfoParser::FFProbeMovieInfoParser(const QString & ffProbeExePath)
    : ffProbePath(ffProbeExePath)
{}

MovieInfo FFProbeMovieInfoParser::parseFile(const QString & movieFilePath) const
{
    MovieInfo mi;
    mi.isFilled = false;

    QStringList args;

    args << movieFilePath
         << "-v" << 0
         << "-select_streams" << "v"
         << "-print_format" << "flat"
         << "-show_entries" << "stream=r_frame_rate,width,height,duration";

    QProcess ffProbe;
    ffProbe.start(ffProbePath, args);

    if(!ffProbe.waitForFinished(15000))
        return mi;

    if(ffProbe.exitCode() != 0)
        return mi;

    QString ffProbeOut = QByteArray(ffProbe.readAll());

    QRegExp rWidth("streams\\.stream\\.0\\.width=(\\d+)");
    if(rWidth.indexIn(ffProbeOut) == -1)
        return mi;
    mi.width = rWidth.cap(1).toInt();

    QRegExp rHeight("streams\\.stream\\.0\\.height=(\\d+)");
    if(rHeight.indexIn(ffProbeOut) == -1)
        return mi;
    mi.height = rHeight.cap(1).toInt();

    QRegExp rFrameRate("streams\\.stream\\.0\\.r_frame_rate=\"(\\d+)/(\\d+)\"");
    if(rFrameRate.indexIn(ffProbeOut) == -1)
        return mi;

    long frL = rFrameRate.cap(1).toLong();
    long frM = rFrameRate.cap(2).toLong();
    mi.frameRate = (double)frL / frM;

    QRegExp rDuration("streams\\.stream\\.0\\.duration=\"(.+)\"");
    if(rDuration.indexIn(ffProbeOut) == -1)
        return mi;

    mi.durationSecs = rDuration.cap(1).toDouble();

    mi.isFilled = true;

    return mi;
}


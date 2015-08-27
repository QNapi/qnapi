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

#ifndef SUBRIPSUBTITLEFORMAT_H
#define SUBRIPSUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class SubRipSubtitleFormat : public SubtitleFormat
{
public:
    virtual ~SubRipSubtitleFormat() {}

    bool isTimeBased() { return true; }
    QString formatName() { return "SRT"; }
    QString defaultExtension() { return "srt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

private:
    struct SrtTimestamps
    {
        int h1;
        int m1;
        int s1;
        int ms1;
        int h2;
        int m2;
        int s2;
        int ms2;
    };

    void addEntry(QVector<SubEntry> & entries, QString & tokensBuff, SrtTimestamps & timestamps);
    QStringList encodeEntry(const SubEntry & entry, int i);
    QString encodeToken(const SubToken & entry);
};

#endif // SUBRIPSUBTITLEFORMAT_H

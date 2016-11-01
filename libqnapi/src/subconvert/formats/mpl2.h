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

#ifndef MPL2SUBTITLEFORMAT_H
#define MPL2SUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class MPL2SubtitleFormat : public SubtitleFormat
{
public:
    virtual ~MPL2SubtitleFormat() {}

    bool isTimeBased() { return true; }
    QString formatName() { return "MPL2"; }
    QString defaultExtension() { return "txt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

private:
    QString encodeEntry(const SubEntry & entry);
    QString encodeToken(const SubToken & entry);
};

#endif // MPL2SUBTITLEFORMAT_H

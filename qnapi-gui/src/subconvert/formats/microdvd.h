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

#ifndef MICRODVD_H
#define MICRODVD_H

#include "subconvert/subtitleformat.h"
#include <QString>

class MicroDVDSubtitleFormat : public SubtitleFormat
{
public:
    virtual ~MicroDVDSubtitleFormat() {}

    bool isTimeBased() { return false; }
    QString formatName() { return "mDVD"; }
    QString defaultExtension() { return "sub"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

private:
    QString encodeEntry(const SubEntry & entry);
    QString encodeToken(const SubToken & entry);
};

#endif // MICRODVD_H


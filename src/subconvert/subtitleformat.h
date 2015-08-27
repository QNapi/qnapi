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

#ifndef SUBTITLEFORMAT_H
#define SUBTITLEFORMAT_H

#include "subconvert/subfile.h"
#include <QString>

class SubtitleFormat
{
public:
    virtual ~SubtitleFormat() {}
    virtual bool isTimeBased() = 0;
    virtual QString formatName() = 0;
    virtual QString defaultExtension() = 0;

    virtual bool detect(const QStringList & lines) = 0;
    virtual SubFile decode(const QStringList & lines) = 0;
    virtual QStringList encode(const SubFile & subFile) = 0;

    QVector<SubToken> decodeTokenStream(QString tokenStream);

};

#endif // SUBTITLEFORMAT_H


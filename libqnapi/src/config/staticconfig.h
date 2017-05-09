/*****************************************************************************
** QNapi
** Copyright (C) 2008-2016 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#ifndef STATICCONFIG_H
#define STATICCONFIG_H

#include <QString>
#include <QStringList>


class StaticConfig
{
public:
    StaticConfig();

    QStringList movieExtensions() const;
    QString movieExtensionsFilter() const;
    QStringList subtitleExtensions() const;
    QString subtitleExtensionsFilter() const;

private:

    QString makeFilter(const QStringList & extensions) const;
};

#endif // STATICCONFIG_H

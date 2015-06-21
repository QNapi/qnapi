/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

#include "synchttp.h"

QNetworkReply* SyncHTTP::syncGet(const QNetworkRequest & req)
{
    QNetworkReply *reply = manager.get(req);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(&manager, SIGNAL(error(QNetworkReply*)), &loop, SLOT(quit()));
    loop.exec();
    return reply;
}

QNetworkReply* SyncHTTP::syncPost(const QNetworkRequest & req, const QByteArray& data)
{
    QNetworkReply *reply = manager.post(req, data);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(&manager, SIGNAL(error(QNetworkReply*)), &loop, SLOT(quit()));
    loop.exec();
    return reply;
}

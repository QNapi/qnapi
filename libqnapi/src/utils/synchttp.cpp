/*****************************************************************************
**  Copyright (C) 2005 by Iulian M
**  eti@erata.net
**
**  modifications by Piotr Krzemiński
**  Copyright (C) 2008-2017
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

SyncHTTP::SyncHTTP() : manager(this) {}

QNetworkReply* SyncHTTP::syncGet(const QNetworkRequest& req) {
  QNetworkReply* reply = manager.get(req);
  connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
          SLOT(quit()));
  loop.exec();
  return reply;
}

QNetworkReply* SyncHTTP::syncPost(const QNetworkRequest& req,
                                  const QByteArray& data) {
  QNetworkReply* reply = manager.post(req, data);
  connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
          SLOT(quit()));
  loop.exec();
  return reply;
}

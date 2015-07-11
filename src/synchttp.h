/*************************************************************************

    Copyright (C) 2005 by Iulian M
    eti@erata.net

    modifications by Piotr Krzemiński
    Copyright (C) 2008-2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef SYNCHTTP_H
#define SYNCHTTP_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QObject>

class SyncHTTP : public QObject
{
    Q_OBJECT
    public:
        SyncHTTP();

        QNetworkReply* syncGet(const QNetworkRequest & req);
        QNetworkReply* syncPost(const QNetworkRequest & req, const QByteArray& data);

    private:
        QEventLoop loop;
        QNetworkAccessManager manager;
};

#endif

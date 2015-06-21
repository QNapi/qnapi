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

#ifndef ETKSYNCHTTP_H
#define ETKSYNCHTTP_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QObject>

class SyncHTTP : public QObject
{
    Q_OBJECT
	public:
        QNetworkReply* syncGet(const QNetworkRequest & req)
		{
            QNetworkReply *reply = manager.get(req);
            connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
            connect(&manager, SIGNAL(error(QNetworkReply*)), &loop, SLOT(quit()));
            loop.exec();
            return reply;
		}

        QNetworkReply* syncPost(const QNetworkRequest & req, const QByteArray& data)
		{
            QNetworkReply *reply = manager.post(req, data);
            connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
            connect(&manager, SIGNAL(error(QNetworkReply*)), &loop, SLOT(quit()));
            loop.exec();
            return reply;
        }

    private:
		QEventLoop loop;
        QNetworkAccessManager manager;
};

#endif

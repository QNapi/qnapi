/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
**
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qmanagedsocket.h"

/*!
    \file qmanagedsocket.cpp

    \brief Implementation of the QManagedSocket class.
*/

#include <QTcpSocket>

QManagedSocket::QManagedSocket(QTcpSocket *s, QObject *p)
    : QObject(p), pSocket(s) {
  if (!s) qFatal("QManagedSocket fed with invalid socket descriptor...");

  connect(s, SIGNAL(readyRead()), this, SLOT(readyRead()));

  connect(s, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

QManagedSocket::~QManagedSocket() {}

void QManagedSocket::send(const QByteArray &msg) { pSocket->write(msg); }

void QManagedSocket::readyRead() {
  if (pSocket) {
    QString msg = QString::fromUtf8(pSocket->readAll());

    emit message(msg);
    emit message(msg, this);
  }
}

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

#ifndef _QMANAGED_SOCKET_H_
#define _QMANAGED_SOCKET_H_

#include "qcumber.h"

/*!
    \file qmanagedsocket.h

    \brief Definition of the QManagedSocket class.
*/

#include <QObject>

class QTcpSocket;

class QCUMBER_EXPORT QManagedSocket : public QObject {
  Q_OBJECT

 public:
  QManagedSocket(QTcpSocket *s, QObject *p = 0);
  virtual ~QManagedSocket();

  virtual void send(const QByteArray &msg);

 signals:
  void message(const QString &m);
  void message(const QString &m, QManagedSocket *s);

 protected slots:
  virtual void readyRead();

 private:
  QTcpSocket *pSocket;
};

#endif  // _QMANAGED_SOCKET_H_

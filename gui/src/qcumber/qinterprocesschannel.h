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

#ifndef _QINTER_PROCESS_CHANNEL_H_
#define _QINTER_PROCESS_CHANNEL_H_

#include "qcumber.h"

/*!
    \file qinterprocesschannel.h

    \brief Definition of the QInterProcessChannel class.
*/

#include <QByteArray>
#include <QHostAddress>
#include <QString>
#include <QThread>

class QTimer;
class QTcpServer;
class QManagedSocket;

class QCUMBER_EXPORT QInterProcessChannel : public QThread {
  Q_OBJECT

 public:
  QInterProcessChannel(QObject* p = 0);
  virtual ~QInterProcessChannel();

  bool isServer() const;

  QString messageBuffer() const;

 public slots:
  void close();
  void reconnect();

  void sendMessage();
  void sendMessage(const QString& s);
  void sendMessage(const QByteArray& b);

  void setMessageBuffer(const QString& s);

 signals:
  void connectionLost();

  void message(const QString& s);
  void request(const QStringList& l);

  void gotServerRole();
  void serverRoleChanged(bool y);

 protected:
  QString rcFile;
  virtual void run();

 private slots:
  void init();
  void check();
  void connection();
  void message(const QString& m, QManagedSocket* s);

 private:
  QString sMsg;
  QHostAddress m_addr;
  quint16 m_port, m_max;

  QTcpServer* pServer;
  QTimer* pServerTimer;
};

#endif  // !_QINTER_PROCESS_CHANNEL_H_

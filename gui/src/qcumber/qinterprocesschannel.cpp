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

#include "qinterprocesschannel.h"

/*!
    \file qinterprocesschannel.cpp

    \brief Implementation of the QInterProcessChannel class.
*/

#include "qmanagedrequest.h"
#include "qmanagedsocket.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#define WAIT_TIMEOUT 5000

/*!
    \class QInterProcessChannel

    \brief A generic network-based communication channel for applications.

    QInterProcessChannel offers communication facilities between several
   instances of the same application. It initializes itself so that the first
   created instance acts as a "server" which recieve messages from the others
   instances. This is particularly handy when creating a single-instance
   application which, for example, uses file association through a command line
   interface.

    \see QSingleApplication
*/

/*!
    \brief Constructor

    Check for a valid server and create one if none found.
*/
QInterProcessChannel::QInterProcessChannel(QObject *p)
    : QThread(p), pServer(0), pServerTimer(0) {
#ifdef Q_OS_WIN
  rcFile = QDir::tempPath() + QDir::separator() +
           QApplication::applicationName() + "rc";
#else
  rcFile = QDir::tempPath() + QDir::separator() +
           QApplication::applicationName() + "-" + getenv("USER") + "-" +
           getenv("DISPLAY") + "-" + "rc";
#endif

  init();
}

/*!
    \brief Destructor
*/
QInterProcessChannel::~QInterProcessChannel() {
  while (isRunning()) quit();

  close();
}

/*!
    \return Wether the instance has a server role
*/
bool QInterProcessChannel::isServer() const { return pServer; }

/*!
    \return The current buffered message
*/
QString QInterProcessChannel::messageBuffer() const { return sMsg; }

/*!
    \brief Sets a buffered message
*/
void QInterProcessChannel::setMessageBuffer(const QString &s) { sMsg = s; }

/*!
    \overload

    Sends the content of the current message buffer
*/
void QInterProcessChannel::sendMessage() {
  sendMessage(sMsg);
  sMsg.clear();
}

/*!
    \overload
*/
void QInterProcessChannel::sendMessage(const QString &s) {
  sendMessage(s.toUtf8());
}
/*!
    \brief Send a message to server instance
*/
void QInterProcessChannel::sendMessage(const QByteArray &msg) {
  if (!pServer && msg.count()) {
    // qDebug("Sending msg : %s", msg.constData());

    QTcpSocket *pSocket = new QTcpSocket(this);
    pSocket->connectToHost(m_addr, m_port);
    pSocket->waitForConnected(WAIT_TIMEOUT);
    pSocket->write(msg);
    pSocket->waitForBytesWritten(WAIT_TIMEOUT);

  } else {
    // qWarning("Empty messages are not carried out...");
  }
}

/*!
    \brief Closes the communication channel

    \see reconnect()
*/
void QInterProcessChannel::close() {
  if (pServer) {
    pServer->close();
    delete pServer;
    pServer = 0;

    QFile::remove(rcFile);
  }
}

/*!
    \brief Attemps to reconnect

    When the server instance is closed (or crashes...) the connectionLost()
   signal is emitted. As an alternative to closing the client instances it is
   possible to try a reconnection so that one of theclient will become a server.

    \note When several clients are running side by side this function is very
   likely to cause a fork and create several (independant) clients whose only
   one will be reachable by newer clients...

    \see connectionLost()
*/
void QInterProcessChannel::reconnect() {
  // qDebug("attempting to reconnect");
  init();
}

/*!
    \internal
*/
void QInterProcessChannel::run() {
  /*
      There we check that the server lives...
      If connection fails notify it...
  */

  if (pServer) return;

  // qDebug("timer setup...");

  pServerTimer = new QTimer;
  pServerTimer->setInterval(100);
  pServerTimer->setSingleShot(false);

  connect(pServerTimer, SIGNAL(timeout()), this, SLOT(check()));

  pServerTimer->start();

  exec();
}

/*!
    \internal
*/
void QInterProcessChannel::check() {
  // qDebug("checking connection...");

  QTcpSocket *pSocket = new QTcpSocket(this);
  pSocket->connectToHost(m_addr, m_port);

  if (pSocket->error() != -1) {
    emit connectionLost();
    return;
  }

  pSocket->waitForConnected(WAIT_TIMEOUT);

  if (pSocket->error() != -1) {
    emit connectionLost();
    return;
  }
}

/*!
    \internal
*/
void QInterProcessChannel::init() {
  while (isRunning()) quit();

  if (pServerTimer) {
    pServerTimer->stop();
    delete pServerTimer;
    pServerTimer = 0;
  }

  bool ok = true;

  m_port = 0;
  m_addr = QHostAddress::LocalHost;

  pServer = new QTcpServer(this);
  pServer->listen(m_addr, m_port);

  connect(pServer, SIGNAL(newConnection()), this, SLOT(connection()));

  if (QFile::exists(rcFile)) {
    /*
        found a server config file, let us assume it is from a running server
    */
    // qDebug("checking old server...");

    QSettings conf(rcFile, QSettings::IniFormat);

    m_port = conf.value("port").toUInt();
    m_addr = QHostAddress(conf.value("address").toString());

    QTcpSocket *pSocket = new QTcpSocket(this);

    if (!m_addr.isNull() && m_port) {
      pSocket->connectToHost(m_addr, m_port);
      ok = pSocket->waitForConnected(WAIT_TIMEOUT);

      if (ok) ok &= (bool)pSocket->write("--check");
      if (ok) ok &= (bool)pSocket->waitForBytesWritten(WAIT_TIMEOUT);
      if (ok) ok &= (bool)pSocket->waitForReadyRead(WAIT_TIMEOUT);
      if (ok) ok &= (pSocket->readAll() == "[ALIVE]");

    } else {
      ok = false;
    }

    if (!ok) {
      QFile::remove(rcFile);
    }

    pSocket->disconnectFromHost();
    delete pSocket;
  }

  if (!QFile::exists(rcFile)) {
    // no server found... Create one
    // qDebug("setting up new server...");

    m_port = pServer->serverPort();
    m_addr = pServer->serverAddress();

    QSettings conf(rcFile, QSettings::IniFormat);
    conf.setValue("port", m_port);
    conf.setValue("address", m_addr.toString());

    emit gotServerRole();
    emit serverRoleChanged(true);
  } else {
    // server found we'll hook on it later on...
    pServer->close();
    pServer = 0;

    emit serverRoleChanged(false);
  }

  start();
}

/*!
    \internal
*/
void QInterProcessChannel::message(const QString &msg, QManagedSocket *s) {
  if (!pServer) return;

  // qDebug("message from Inter Process channel : %s", qPrintable(msg));

  QStringList argv = QManagedRequest::splitArguments(msg);
  const QString cmd = argv.at(0);
  // const int argc = argv.count();

  if (cmd == "--check") {
    s->send("[ALIVE]");
  } else if (cmd == "--request") {
    argv.removeAt(0);
    emit request(argv);
  } else {
    emit message(msg);
  }
}

/*!
    \brief internal
*/
void QInterProcessChannel::connection() {
  if (!pServer) return;

  // qDebug("incoming Inter Process connection");

  while (pServer->hasPendingConnections()) {
    QManagedSocket *s =
        new QManagedSocket(pServer->nextPendingConnection(), this);

    connect(s, SIGNAL(message(QString, QManagedSocket *)), this,
            SLOT(message(QString, QManagedSocket *)));
  }
}

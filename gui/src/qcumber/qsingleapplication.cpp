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

#include "qsingleapplication.h"

#include "qinterprocesschannel.h"
#include "qmanagedrequest.h"

#include <QStringList>

/*!
    \brief Constructor
*/
QSingleApplication::QSingleApplication(int& argc, char** argv, bool useGui,
                                       const QString& appName)
    : QApplication(argc, argv, useGui) {
  setApplicationName(appName);

  pChannel = new QInterProcessChannel(this);

  connect(pChannel, SIGNAL(message(QString)), this, SLOT(message(QString)));

  connect(pChannel, SIGNAL(request(QStringList)), this,
          SLOT(request(QStringList)));

  setMessagingPolicy(Signals);
  setInstanciationPolicy(None);
}

/*!
    \brief Destructor
*/
QSingleApplication::~QSingleApplication() { pChannel->close(); }

/*!
    \return Wheter the creation of an instance is allowed.
*/
bool QSingleApplication::isInstanceAllowed() const {
  return pChannel->isServer();
}

/*!
    \return The current messaging policy
*/
QSingleApplication::MessagingPolicy QSingleApplication::messagingPolicy()
    const {
  return m_messaging;
}

/*!
    \brief Sets the messaging policy

    \see messagingPolicy()
*/
void QSingleApplication::setMessagingPolicy(MessagingPolicy p) {
  m_messaging = p;
}

/*!
    \return The current instanciation policy
*/
QSingleApplication::InstanciationPolicy
QSingleApplication::instanciationPolicy() const {
  return m_instanciation;
}

/*!
    \brief Sets the instanciation policy

    \see instanciationPolicy()
*/
void QSingleApplication::setInstanciationPolicy(InstanciationPolicy p) {
  m_instanciation = p;
}

/*!
    \brief Launchs the application if allowed

    \see instanciationPolicy()
*/
int QSingleApplication::exec() {
  int ret = -1;

  if (isInstanceAllowed()) {
    ret = QApplication::exec();

  } else {
    switch (instanciationPolicy()) {
      case ForwardArguments:
        qWarning("QSingleApplication : forwarding parameters");
        sendRequest(arguments());
        break;

      default:
        qWarning("QSingleApplication : all instances already occupied");
        break;
    }
  }

  return ret;
}

/*!
    \overload
*/
void QSingleApplication::sendRequest(const QString& s) {
  sendRequest(QStringList(s));
}

/*!
    \brief sends a message to the running instance of the application

    \param l A list of arguments that make up a request to be sent.
*/
void QSingleApplication::sendRequest(const QStringList& l) {
  if (!pChannel || l.isEmpty()) return;

  /*
  qDebug("sending message to %s:%i over TCP/IP : \"%s\"",
          qPrintable(pManager->address().toString()),
          pManager->port(),
          msg.constData());
  */

  pChannel->sendMessage(QManagedRequest::joinArguments("--request", l));
}

/*!
    \internal
*/
void QSingleApplication::message(const QString& s) {
  if (s.isEmpty()) return;

  switch (messagingPolicy()) {
    case Events:

      /*
      QRequestEvent *e = new QRequestEvent(s);
      postEvent(s);
      */

      break;

    case Signals:
      emit request(s);
      break;

    default:
      break;
  }
}

/*!
    \internal
*/
void QSingleApplication::request(const QStringList& l) {
  if (l.isEmpty()) return;

  switch (messagingPolicy()) {
    case Events:

      /*
      QRequestEvent *e = new QRequestEvent(s);
      postEvent(s);
      */

      break;

    case Signals:
      emit request(l.join(" | "));
      break;

    default:
      break;
  }
}

/*!
    \internal
*/
bool QSingleApplication::event(QEvent* e) { return QApplication::event(e); }

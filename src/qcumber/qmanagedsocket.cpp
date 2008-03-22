/****************************************************************************
**
** 	Created using Edyuk IDE 0.8.0
**
** File : qmanagedsocket.cpp
** Date : Sat Feb 3 13:22:44 2007
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
 : QObject(p), pSocket(s)
{
	if ( !s )
		qFatal("QManagedSocket fed with invalid socket descriptor...");
	
	connect(s	, SIGNAL( readyRead() ),
			this, SLOT  ( readyRead() ) );
	
	connect(s	, SIGNAL( disconnected() ),
			this, SLOT  ( deleteLater() ) );
	
}

QManagedSocket::~QManagedSocket()
{
	
}

void QManagedSocket::send(const QByteArray& msg)
{
	pSocket->write(msg);
}

void QManagedSocket::readyRead()
{
	if ( pSocket )
	{
		QString msg = QString::fromLocal8Bit(pSocket->readAll());
		
		emit message(msg);
		emit message(msg, this);
	}
}

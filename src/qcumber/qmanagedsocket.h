/****************************************************************************
**
** 	Created using Edyuk IDE 0.8.0
**
** File : qmanagedsocket.h
** Date : Sat Feb 3 13:22:37 2007
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

class QCUMBER_EXPORT QManagedSocket : public QObject
{
	Q_OBJECT
	
	public:
		QManagedSocket(QTcpSocket *s, QObject *p = 0);
		virtual ~QManagedSocket();
		
		virtual void send(const QByteArray& msg);
		
	signals:
		void message(const QString& m);
		void message(const QString& m, QManagedSocket *s);
		
	protected slots:
		virtual void readyRead();
		
	private:
		QTcpSocket *pSocket;
};

#endif // _QMANAGED_SOCKET_H_

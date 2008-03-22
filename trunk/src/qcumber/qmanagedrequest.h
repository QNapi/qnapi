/****************************************************************************
**
** 	Created using Edyuk IDE 0.8.0
**
** File : qmanagedrequest.h
** Date : Sat Feb 3 13:53:27 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QMANAGED_REQUEST_H_
#define _QMANAGED_REQUEST_H_

#include "qcumber.h"

/*!
	\file qmanagedrequest.h
	
	\brief Definition of the QManagedRequest class.
*/

#include <QStringList>

class QCUMBER_EXPORT QManagedRequest
{
	public:
		QManagedRequest(const QString& cmd, const QStringList& args = QStringList());
		QManagedRequest(const QManagedRequest& r);
		~QManagedRequest();
		
		static QManagedRequest createSystemRequest(const QStringList& args, int id = -1);
		
		QManagedRequest& operator = (const QManagedRequest& r);
		
		QString toString() const;
		static QManagedRequest fromString(const QString& s);
		
		static QStringList splitArguments(const QString& s);
		static QByteArray joinArguments(const QString& cmd, const QStringList& l);
		
	private:
		QString sCommand;
		QStringList lArguments;
};

#endif // _QMANAGED_REQUEST_H_

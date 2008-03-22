/****************************************************************************
**
** 	Created using Edyuk IDE 0.8.0
**
** File : qmanagedrequest.cpp
** Date : Sat Feb 3 13:53:34 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qmanagedrequest.h"

/*!
	\file qmanagedrequest.cpp
	
	\brief Implementation of the QManagedRequest class.
*/

QManagedRequest::QManagedRequest(const QString& cmd, const QStringList& args)
 : sCommand(cmd), lArguments(args)
{
	
}

QManagedRequest::QManagedRequest(const QManagedRequest& r)
 : sCommand(r.sCommand), lArguments(r.lArguments)
{
	
}

QManagedRequest::~QManagedRequest()
{
	sCommand.clear();
	lArguments.clear();
}

QManagedRequest QManagedRequest::createSystemRequest(const QStringList& args, int id)
{
	QManagedRequest r("--request");
	r.lArguments = args;
	r.lArguments.prepend( QString::number(id) );
	
	return r;
}

QManagedRequest& QManagedRequest::operator = (const QManagedRequest& r)
{
	sCommand = r.sCommand;
	lArguments = r.lArguments;
	
	return *this;
}

QString QManagedRequest::toString() const
{
	return QString::fromLocal8Bit(joinArguments(sCommand, lArguments));
}

QManagedRequest QManagedRequest::fromString(const QString& s)
{
	QManagedRequest r("");
	r.lArguments = splitArguments(s);
	r.sCommand = r.lArguments.takeAt(0);
	
	return r;
}

QStringList QManagedRequest::splitArguments(const QString& s)
{
	int i = -1;
	QStringList d, l = s.split(QRegExp("\\s"));
	
	while ( ++i < l.count() )
	{
		if ( l.at(i).startsWith("\"") )
		{
			QString arg;
			
			do
			{
				arg += l.at(i);
			} while ( !arg.endsWith("\"") && (++i < l.count()) );
			
			arg.remove(0, 1);
			
			if ( arg.endsWith("\"") )
				arg.chop(1);
			
			d << arg;
		} else {
			d << l.at(i);
		}
	}
	
	return d;
}

QByteArray QManagedRequest::joinArguments(const QString& cmd, const QStringList& l)
{
	QByteArray msg;
	
	msg += cmd.toLocal8Bit();
	
	foreach ( QString a, l )
	{
		if ( a.isEmpty() )
			continue;
		
		if ( a.contains(QRegExp("\\s")) )
			a = "\"" + a + "\"";
		
		msg += " ";
		msg += a.toLocal8Bit();
	}
	
	return msg;
}

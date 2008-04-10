/****************************************************************************
**
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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
#include "qmanagedrequest.h"

#include <QThread>
#include <QString>
#include <QByteArray>
#include <QApplication>

#include <windows.h>

/*!
	\file qinterprocesschannel.h
	
	\brief Definition of the QInterProcessChannel class.
*/

// buffer size for input/output pipes
static const int bufferSize = 2048;

class QCUMBER_EXPORT QInterProcessChannel : public QThread
{
	Q_OBJECT
	
	public:
		QInterProcessChannel(QObject *p = 0);
		virtual ~QInterProcessChannel();
		
		bool isServer() const;
		
		QString messageBuffer() const;
		
	public slots:
		void close();
		
		void sendMessage();
		void sendMessage(const QString& s);
		void sendMessage(const QByteArray& b);
		
		void setMessageBuffer(const QString& s);
		
	signals:
		void message(const QString& s);
		void request(const QStringList& l);
		
	protected:
		virtual void run();
		
	private slots:
		void init();
		
	private:
		QString uniqPrefix, pipeName, globalMutexStr, blockerMutexStr;
		QString sMsg;
		bool serverMode;
		
};

#endif // _QINTER_PROCESS_CHANNEL_H_

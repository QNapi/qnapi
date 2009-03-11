/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#ifndef __QSYNCXMLRPCCLIENT__H__
#define __QSYNCXMLRPCCLIENT__H__

#include "client.h"

#include <QEventLoop>

class QSyncXmlRpcClient : public xmlrpc::Client
{
	Q_OBJECT

	public:

		QSyncXmlRpcClient(QObject * parent = 0);
		QSyncXmlRpcClient(	const QString & hostname, quint16 port = 80,
							QObject * parent = 0L);
		~QSyncXmlRpcClient();

		void connectSignals();
		void setRequestTimeout(int t);

		// sync requests; returns 1 if success or 0 if fails
		int request(QList<xmlrpc::Variant> params, QString methodName);
		int request(QString methodName);
		int request(QString methodName, xmlrpc::Variant param1);
		int request(QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2);
		int request(QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2,
					xmlrpc::Variant param3);
		int request(QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2,
					xmlrpc::Variant param3, xmlrpc::Variant param4);

		QVariant & result();

	protected slots:

		void requestDone(int idx, QVariant res);
		void requestFailed(int idx, int errorCode, QString errorString);
		void abort();

	private:

		QEventLoop loop;
		QTimer timeoutTimer;
		int timeout;
		int requestID, status, errCode;
		QString errString;
		QVariant resultVar;

};

#endif

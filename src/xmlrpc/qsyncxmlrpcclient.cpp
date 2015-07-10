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

#include "qsyncxmlrpcclient.h"

QSyncXmlRpcClient::QSyncXmlRpcClient(QObject * parent)
            : xmlrpc::Client(parent)
{
    timeout = 30000; // 30 s. timeoutu
    connectSignals();
}

QSyncXmlRpcClient::QSyncXmlRpcClient(const QString & hostname, quint16 port, QObject * parent)
            : xmlrpc::Client(hostname, port, parent)
{
    connectSignals();
}

QSyncXmlRpcClient::~QSyncXmlRpcClient() {};

void QSyncXmlRpcClient::connectSignals()
{
    connect(this, SIGNAL(done(int, QVariant)), this, SLOT(requestDone(int, QVariant)));
    connect(this, SIGNAL(failed(int, int, QString)), this, SLOT(requestFailed(int, int, QString)));
    connect(&timeoutTimer, SIGNAL(timeout()),  this, SLOT(abort()));
}

void QSyncXmlRpcClient::setRequestTimeout(int t)
{
    timeout = t;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QList<xmlrpc::Variant> params, QString methodName )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(params, methodName);
    loop.exec();
    return status;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QString methodName )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(methodName);
    loop.exec();
    return status;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QString methodName, xmlrpc::Variant param1 )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(methodName, param1);
    loop.exec();
    return status;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QString methodName, xmlrpc::Variant param1,
                                xmlrpc::Variant param2  )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(methodName, param1, param2);
    loop.exec();
    return status;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QString methodName, xmlrpc::Variant param1,
                                xmlrpc::Variant param2, xmlrpc::Variant param3  )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(methodName, param1, param2, param3);
    loop.exec();
    return status;
}

// sync request. returns 1 if success or 0 if fails
int QSyncXmlRpcClient::request( QString methodName, xmlrpc::Variant param1,
                                xmlrpc::Variant param2, xmlrpc::Variant param3,
                                xmlrpc::Variant param4 )
{
    timeoutTimer.start(timeout);
    requestID = xmlrpc::Client::request(methodName, param1, param2, param3, param4);
    loop.exec();
    return status;
}

QVariant & QSyncXmlRpcClient::result()
{
    return resultVar;
}

void QSyncXmlRpcClient::requestDone(int idx, QVariant res)
{
    if(idx != requestID)
        return;
    status = 1;
    resultVar = res;
    loop.exit();
}

void QSyncXmlRpcClient::requestFailed(int idx, int errorCode, QString errorString)
{
    if(idx != requestID)
        return;
    errCode = errorCode;
    errString = errorString;
    status = 0;
    loop.exit();
}

void QSyncXmlRpcClient::abort()
{
    status = 0;
    requestFailed(requestID, 0, "timeout");
    loop.exit();
}

// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2005 Dmitry Poplavsky <dima@thekompany.com>

#ifndef CLIENT_H
#define CLIENT_H

#include <qobject.h>
#include <QHttp>

#include "xmlrpc/variant.h"

namespace  xmlrpc {

/*!
 \class xmlrpc::Client client.h
 \brief The xmlrpc::Client class provides an implementation of the XML-RPC client.

 The class works asynchronously, so there are no blocking functions.

 Each XML-RPC request has unique identifier, which is returned by xmlrpc::Client::request()
 and is emited with done() and failed() signals.



\code
 client = new xmlrpc::Client(this);
 connect( client, SIGNAL(done( int, QVariant )),
          this, SLOT(processReturnValue( int, QVariant )) );
 connect( client, SIGNAL(failed( int, int, QString )),
          this, SLOT(processFault( int, int, QString )) );
 
 client->setHost( "localhost", 7777 );
 
 int requestId = client->request( "sum", x, y )
 
\endcode

 After the request is finished, done() or failed() signal
 will be emited with the request id and return value or fault information.
 */

class Client : public QObject {
Q_OBJECT
public:
	Client(QObject * parent = 0);
	Client(const QString & hostname, quint16 port = 80, QObject * parent = 0L);

    void setHost ( const QString & hostname, quint16 port = 80, QString path="/" );
    void setProxy ( const QString & host, int port, 
                    const QString & username = QString(), const QString & password = QString() );
    void setSocket ( QTcpSocket * socket );
    void setUser ( const QString & userName, const QString & password = QString() );

    void setUserAgent( const QString & userAgent );

	virtual ~Client();

    int request( QList<Variant> params, QString methodName );

    /* overloaded methods */
    int request( QString methodName );
    int request( QString methodName, Variant param1 );
    int request( QString methodName, Variant param1, Variant param2 );
    int request( QString methodName, Variant param1, Variant param2, Variant param3 );
    int request( QString methodName, Variant param1, Variant param2, Variant param3, Variant param4 );

signals:
    //! request requestId is done with return value res
    void done( int requestId, QVariant res );
    //! request requestId is failed with fault code faultCode and fault description faultString
    void failed( int requestId, int faultCode, QString faultString );

    //! authenticationRequired signal passed from QHttp
    void authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * );

    //! proxyAuthenticationRequired signal passed from QHttp
    void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);

protected slots:
    void requestFinished(int id, bool error);

private:
	class Private;
	Private *d;
}; 

} // namespace

#endif // CLIENT_H



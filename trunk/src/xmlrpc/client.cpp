// vim:tabstop=4:shiftwidth=4:foldmethod=marker:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2005 Dmitry Poplavsky <dima@thekompany.com>

#include "xmlrpc/client.h"
#include "xmlrpc/request.h"
#include "xmlrpc/response.h"

#include <qhttp.h>
#include <qbuffer.h>
#include <QtNetwork>

//#define XMLRPC_DEBUG

namespace  xmlrpc {

class Client::Private
{
public:
    QString hostName;
    quint16 port;
    QString path;

    QString userName;
    QString password;

    QString userAgent;

    QHttp *http;

    QAuthenticator proxyAuth;

    QMap<int,QBuffer*> serverResponses;
    QMap<int, QString> methodNames; // id->methodNames
};

/**
 * Constructs a XmlRPC client.
 */
Client::Client(QObject * parent)
: QObject( parent )
{
    d = new Private;
    d->port = 0;
    d->path = "/";
    d->userAgent = "QXMLRPC";
    d->http = new QHttp(this);

    connect( d->http, SIGNAL(requestFinished(int,bool)), SLOT(requestFinished(int,bool)) );

    connect( d->http, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),
             this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *) ) );

    connect( d->http, SIGNAL( authenticationRequired ( const QString &, quint16, QAuthenticator * ) ),
             this, SIGNAL( authenticationRequired ( const QString &, quint16, QAuthenticator * ) ) );
}

/**
 * Constructs a XmlRPC client for communication with XmlRPC
 * server running on host \a hostName \a port.
 */
Client::Client(const QString & hostName, quint16 port, QObject * parent)
: QObject( parent )
{
    d = new Private;
    setHost( hostName, port );

    //important: dissconnect all connection from http in destructor,
    //otherwise crashes are possible when other parts of Client::Private
    //is deleted before http
    connect( d->http, SIGNAL(requestFinished(int,bool)), SLOT(requestFinished(int,bool)) );
}

/**
 * Destroys the XmlRPC client.
 */
Client::~Client()
{
    // it's necessary to delete QHttp instance before Private instance
    // to be sure Client slots will not be called with already deleted Private data
    delete d->http;
    delete d;
}


/**
 * Sets the XML-RPC server that is used for requests to hostName
 * on port \a port and path \path.
 */
void Client::setHost( const QString & hostName, quint16 port, QString path )
{
    d->hostName = hostName;
    d->port = port;
    d->path = path;
    d->http->setHost( hostName, port );
}

/**
 * Enables HTTP proxy support, using the proxy server host on port port.
 * username and password can be provided if the proxy server
 * requires authentication.
 */
void Client::setProxy( const QString & host, int port, 
                    const QString & userName, const QString & password )
{

#ifdef XMLRPC_DEBUG
    qDebug() << "xmlrpc client: set proxy" << host << port << userName << password;
#endif

    d->http->setProxy( host, port, userName, password );
}

/**
 * Replaces the internal QTcpSocket that QHttp uses with socket.
 * This can be useful for adding https support with QtSslSocket.
 * 
 * Check
 * \l{http://trolltech.com/products/qt/addon/solutions/catalog/4/Utilities/qtsslsocket/}
 * and QHttp::setSocket() for more information.
 */
void Client::setSocket( QTcpSocket * socket )
{
    d->http->setSocket( socket );
}

/**
 * Set the user name userName and password password for XML-RPC
 * ( or http ) server that require authentication.
 */
void Client::setUser( const QString & userName, const QString & password )
{
    //d->http->setUser( userName, password );
    d->userName = userName;
    d->password = password;
}

/**
 * Set the user agent HTTP value instead of default "QXMLRPC"
 */
void Client::setUserAgent( const QString & userAgent )
{
    d->userAgent = userAgent;
}

/**
 * Call method methodName on server side with parameters list
 * params. Returns id of request, used in done() and failed()
 * signals.
 * 
 * The parameters order is changed in overloaded methods to
 * avoid situation when the only parameter is the list.
 * \code
 * QList<xmlrpc::Variant> parameter;
 * ...
 * int requestId = client->request( methodName, parameter );
 * \endcode
 * This leads to this method be called, with parameter treated
 * as parameters list. It's possible to fix this with next code:
 * \code
 * client->request( methodName, xmlrpc::Variant(parameter) );
 * \endcode
 * but to avoid such kind of bugs, the parameters order in
 * overloaded methods was changed.
 */
int Client::request( QList<Variant> params, QString methodName )
{
    QBuffer *outBuffer = new QBuffer;

    QByteArray data = Request(methodName,params).composeRequest();

    QHttpRequestHeader header("POST",d->path);
    header.setContentLength( data.size() );
    header.setContentType("text/xml");
    header.setValue( "User-Agent", d->userAgent );
    header.setValue( "Connection", "Keep-Alive");


    if ( !d->userName.isEmpty() ) {
        QByteArray authData = QString(d->userName + ":" + d->password).toUtf8();
        authData = authData.toBase64();
        authData = QByteArray("Basic ")+authData;
        header.setValue("Authorization", authData.data() );
    }

    //header.setValue("Connection", "close");

    header.setValue("host",d->hostName);

    //d->http->setHost( d->hostName, d->port );

    int id = d->http->request( header, data, outBuffer );
    d->serverResponses[id] = outBuffer;
    d->methodNames[id] = methodName;
    d->http->close();

#ifdef XMLRPC_DEBUG
    qDebug() << "xmlrpc request(" << id << "): " << methodName;
    qDebug() << Variant(params).pprint();
#endif

    return id;
}

/**
 * Call method methodName on server side with empty parameters
 * list. This is an overloaded member function, provided for
 * convenience.
 */
int Client::request( QString methodName )
{
    QList<xmlrpc::Variant> params;
    return request( params, methodName );
}

/**
 * Call method methodName on server side with one parameter.
 * This is an overloaded member function, provided for
 * convenience.
 */
int Client::request( QString methodName, Variant param1 )
{
    QList<xmlrpc::Variant> params;
    params << param1;
    return request( params, methodName );
}

/**
 * Call method methodName on server side with two parameters.
 * This is an overloaded member function, provided for
 * convenience.
 */
int Client::request( QString methodName, Variant param1, Variant param2 )
{
    QList<xmlrpc::Variant> params;
    params << param1 << param2;
    return request( params, methodName );
}

/**
 * Call method methodName on server side with three parameters.
 * This is an overloaded member function, provided for
 * convenience.
 */
int Client::request( QString methodName, Variant param1, Variant param2, Variant param3 )
{
    QList<xmlrpc::Variant> params;
    params << param1 << param2 << param3;
    return request( params, methodName );
}
/**
 * Call method methodName on server side with four parameters.
 * This is an overloaded member function, provided for
 * convenience.
 */
int Client::request( QString methodName, Variant param1, Variant param2, Variant param3, Variant param4 )
{
    QList<xmlrpc::Variant> params;
    params << param1 << param2 << param3 << param4;
    return request( params, methodName );
}

void Client::requestFinished(int id, bool error)
{
    if ( !d->serverResponses.count(id) ) {
        return;
    }

#ifdef XMLRPC_DEBUG
    qDebug() << "request" <<  d->methodNames[id] <<  "finished, id=" << id << ", isError:" << error;
#endif

    if ( error ) {
        //if ( d->serverResponses.count(id) )

        d->serverResponses.take(id);

        emit failed(id, -32300, d->http->errorString() );
        return;
    }

    if ( d->serverResponses.count(id) ) {
        QByteArray buf = d->serverResponses.take(id)->buffer();

        //qDebug() << "xml-rpc server response:\n" << QString(buf);

        Response response;

        QString errorMessage;
        if ( response.setContent( buf, &errorMessage ) ) {
            Q_ASSERT( !response.isNull() );

            if ( response.isFault() ) {
                qDebug() << "request failed:" << response.faultCode() << response.faultString();
                emit failed(id, response.faultCode(), response.faultString() );
            } else {
#ifdef XMLRPC_DEBUG
                qDebug() << response.returnValue().pprint();
#endif
                emit done( id, response.returnValue() );
            }

        } else {

#ifdef XMLRPC_DEBUG
            qDebug() << "incorrect xmlrpc response:" << errorMessage;
            qDebug() << QString(buf);
#endif
            emit failed(id, -32600, "Server error: Invalid xml-rpc. \nNot conforming to spec.");
        }

    }
	
}


} 



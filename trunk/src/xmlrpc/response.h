// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2007
// Author Dmitry Poplavsky <dmitry.poplavsky@gmail.com>

#ifndef RESPONSE_H
#define RESPONSE_H

#include "variant.h"

namespace xmlrpc {

/**
 * \brief The xmlrpc::Response class contains XmlRPC response
 *        information.
 * 
 * xmlrpc::Response class is usually not intended to be used
 * directly but from the xmlrpc::Server and xmlrpc::Client,
 * except of building custom server implementation.
 */
class Response {
public:
    Response();
    Response( const Variant& returnValue );
    Response( int faultCode, const QString& faultString );

    virtual ~Response();

    bool isNull() const;
    
    bool isFault() const;
    int faultCode() const;
    QString faultString() const;

    Variant returnValue() const;


    bool setContent( const QByteArray& requestData, QString *errorMessage = 0 );
    QByteArray composeResponse() const;

private:
    class Private;
	QSharedDataPointer<Private> d;
};

} // namespace xmlrpc

#endif // ifndef RESPONSE_H


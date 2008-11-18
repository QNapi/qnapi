// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2007
// Author Dmitry Poplavsky <dmitry.poplavsky@gmail.com>

#ifndef REQUEST_H
#define REQUEST_H

#include "variant.h"

namespace xmlrpc {

/**
 * \brief The xmlrpc::Request class contains XmlRPC request
 *        information.
 * 
 * xmlrpc::Request class is usually not intended to be used
 * directly but from the xmlrpc::Server and xmlrpc::Client,
 * except of building custom server implementation.
 */
class Request {
public:
    Request();
    Request( const QString& methodName, const QList<Variant>& );

    ~Request();

    bool isNull() const;

    QString methodName() const;
    QList<Variant> parameters() const;

    bool setContent( const QByteArray& requestData, QString *errorMessage = 0 );
    QByteArray composeRequest() const;

private:
    class Private;
	QSharedDataPointer<Private> d;
};

} // namespace xmlrpc

#endif // ifndef REQUEST_H


// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2007
// Author Dmitry Poplavsky <dmitry.poplavsky@gmail.com>

#include "request.h"

#include <QtXml>

using namespace xmlrpc;

class Request::Private : public QSharedData
{
public:
    Private() 
        :isNull(true) {};

    Private( const Private& other ) 
        :QSharedData(other)
    {
        isNull = other.isNull;
        methodName = other.methodName;
        parameters = other.parameters;
    }

    
    bool isNull;

    QString methodName;
    QList<Variant> parameters;
};

/**
 * Construct an emty XmlRPC request.
 */
Request::Request()
{
    d = new Private();
}

/**
 * Construct XmlRPC request with methon name \a methodName and
 * parameters list \a parameters.
 */
Request::Request( const QString& methodName, const QList<Variant>& parameters )
{
    d = new Private();
    d->isNull = false;
    d->methodName = methodName;
    d->parameters = parameters;
}

Request::~Request()
{
}

/**
 * Returns true if the request is null ( not initialised );
 * otherwise returns false.
 **/
bool Request::isNull() const
{
    return d->isNull;
}

/**
 * Return method name of request
 */
QString Request::methodName() const
{
    return d->methodName;
}

/**
 * Return parameters list of request
 */
QList<Variant> Request::parameters() const
{
    return d->parameters;
}


/**
 * This function parses the XML-RPC request from the byte array
 * \a requestData. If a parse error occurs, this function
 * returns false and the error message is placed in
 * \c{*}\a{errorMessage}.
 */
bool Request::setContent( const QByteArray& requestData, QString *errorMessage )
{
    d->isNull = true;
    d->methodName.clear();
    d->parameters.clear();

    QDomDocument doc;

    QString domErrorMsg;
    int domErrorLine;
    int domErrorColumn;

    if ( !doc.setContent( requestData, &domErrorMsg, &domErrorLine, &domErrorColumn) ) {
        if ( errorMessage ) {
            *errorMessage = QString("%1\nline: %2 column:%3")\
                                .arg(domErrorMsg)\
                                .arg(domErrorLine)\
                                .arg(domErrorColumn);
        }

        return false;
    }

    QDomElement methodCall = doc.firstChildElement("methodCall");
    QDomElement methodNameElement = methodCall.firstChildElement("methodName");
    d->methodName = methodNameElement.text();

    if ( d->methodName.isEmpty() ) {
        if ( errorMessage ) 
            *errorMessage = QString("Method name is missing");

        return false;
    }

    QDomElement paramsElement = methodCall.firstChildElement("params");

    if ( paramsElement.isNull() ) {
        if ( errorMessage ) 
            *errorMessage = QString("Parameters are missing");

        return false;
    }

    QDomElement paramElement = paramsElement.firstChildElement("param");
    while ( !paramElement.isNull() ) {
        QDomElement valueElement = paramElement.firstChildElement("value");
        Variant v( valueElement );

        if ( !v.isValid() ) {
            if ( errorMessage ) {
                QString variableXmlText;
                *errorMessage = QString("Invalid parameter: %1").arg(variableXmlText);
            }

            d->parameters.clear();
            return false;
        }

        d->parameters.append( v );
        paramElement = paramElement.nextSiblingElement("param");
    }

    d->isNull = false;
    return true;
}

/**
 * Compose XmlRPC request data.
 */
QByteArray Request::composeRequest() const
{
    Q_ASSERT( !isNull() );

    QDomDocument doc;
    QDomElement methodCall = doc.createElement("methodCall");
    doc.appendChild( methodCall );

    QDomElement methodNameNode = doc.createElement("methodName");
    methodNameNode.appendChild( doc.createTextNode( d->methodName ) );
    methodCall.appendChild( methodNameNode );

    QDomElement paramsNode = doc.createElement("params");
    methodCall.appendChild( paramsNode );

    foreach ( Variant parameter, d->parameters ) {
        QDomElement paramNode = doc.createElement("param");
        paramNode.appendChild( parameter.toDomElement(doc) );
        paramsNode.appendChild( paramNode );
    }

    return doc.toByteArray(2);
}


// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2007
// Author Dmitry Poplavsky <dmitry.poplavsky@gmail.com>

#include "response.h"

#include <QtXml>

using namespace xmlrpc;

class Response::Private : public QSharedData {
public:
    Private() 
    :isNull(true), isFault(false), faultCode(0) {};

    Private( const Private& other ) 
    :QSharedData(other)
    {
        isNull = other.isNull;

        isFault = other.isFault;

        faultCode = other.faultCode;
        faultString = other.faultString;

        returnValue = other.returnValue;

    }

    bool isNull;

    bool isFault;
    int faultCode;
    QString faultString;

    Variant returnValue;
};

/**
 * Construct an emty XmlRPC request.
 */
Response::Response()
{
    d = new Private();
}

/**
 * Construct XmlRPC successful response with returnValue value
 */
Response::Response( const Variant& returnValue )
{
    d = new Private();
    d->isNull = false;

    d->isFault = false;

    d->returnValue = returnValue;
}

/**
 * Construct XmlRPC failed response with faultCode and
 * faultString values
 */
Response::Response( int faultCode, const QString& faultString )
{
    d = new Private();
    d->isNull = false;

    d->isFault = true;

    d->faultCode = faultCode;
    d->faultString = faultString;
}

Response::~Response()
{
}

/**
 * Returns true if the response is null ( not initialised );
 * otherwise returns false.
 **/
bool Response::isNull() const
{
    return d->isNull;
}

/**
 * Returns true is this response is fault response.
 */
bool Response::isFault() const
{
    return d->isFault;
}

/**
 * Returns fault code assotiated with this response.
 * 
 * It is valid only for fault responses.
 */
int Response::faultCode() const
{
    Q_ASSERT( isFault() );
    return d->faultCode;
}

/**
 * Returns fault string assotiated with this response.
 * 
 * It is valid only for fault responses.
 */
QString Response::faultString() const
{
    Q_ASSERT( isFault() );
    return d->faultString;
}

/**
 * Returns return value assotiated with this response.
 * 
 * It is valid only for not fault responses.
 */
Variant Response::returnValue() const
{
    Q_ASSERT( !isFault() );
    return d->returnValue;
}


/**
 * This function parses the XML-RPC response from the byte array
 * \a requestData. If a parse error occurs, this function
 * returns false and the error message is placed in * \a
 * errorMessage.
 */
bool Response::setContent( const QByteArray& responseData, QString *errorMessage )
{
    d->isNull = true;

    QDomDocument doc;

    QString domErrorMsg;
    int domErrorLine;
    int domErrorColumn;



    if ( !doc.setContent( responseData, &domErrorMsg, &domErrorLine, &domErrorColumn) ) {
        if ( errorMessage ) {
            *errorMessage = QString("%1\nline: %2 column:%3")\
                            .arg(domErrorMsg)\
                            .arg(domErrorLine)\
                            .arg(domErrorColumn);
        }

        return false;
    }

    QString errorString;


    do {
        QDomElement methodResponse = doc.firstChildElement("methodResponse");

        if ( methodResponse.isNull() ) {
            errorString = "Missing methodResponse tag";
            break;
        }

        QDomElement resNode = methodResponse.firstChildElement();

        if ( resNode.tagName() == "params" ) {
            // the call was successful

            d->isFault = false;

            QDomElement paramValue = resNode.firstChild().firstChild().toElement();

            d->returnValue = Variant( paramValue );

            if ( !d->returnValue.isValid() ) {
                errorString = "Invalid return value";
                break;
            }

            d->isNull = false;
            break;
        };


        if ( resNode.tagName() == "fault" ) {
            // server returned fault code, return error id and string

            d->isFault = true;

            QDomElement paramValue = resNode.firstChildElement();

            Variant res(paramValue); // should be a map
            if ( res.type() != QVariant::Map ) {
                errorString = QString("Expected struct for xmlrpc fault, received %1").arg( paramValue.firstChildElement().nodeName() );
                break;
            }

            QMap<QString,QVariant> map = res.toMap();

            if ( !map.contains("faultCode") || !map.contains("faultString") ) {
                errorString = "Fault struct doesn't contain fault code or string values";
                break;
            }

            d->faultCode = map["faultCode"].toInt();
            d->faultString = map["faultString"].toString();

            d->isNull = false;
            break;
        }

        errorString = QString("Unexpected tag: %1").arg( resNode.tagName() );

    } while (0);


    if ( d->isNull ) {
        if ( errorMessage ) {
            *errorMessage = errorString;
            return false;
        }
    }

    return true;
}

/**
 * Compose XmlRPC response data.
 */
QByteArray Response::composeResponse() const
{
    Q_ASSERT( !isNull() );

    QDomDocument doc;


    QDomElement methodResponse = doc.createElement("methodResponse");
    doc.appendChild( methodResponse );

    if ( isFault() ) {
        QDomElement fault = doc.createElement("fault");
        methodResponse.appendChild( fault );

        QMap<QString, Variant> faultValue;
        faultValue[ "faultCode" ] = faultCode();
        faultValue[ "faultString" ] = faultString();

        fault.appendChild( Variant(faultValue).toDomElement(doc) );
    } else {
        QDomElement params = doc.createElement("params");
        methodResponse.appendChild( params );
        QDomElement param = doc.createElement("param");
        params.appendChild( param );

        param.appendChild( d->returnValue.toDomElement(doc) );
    }

    return doc.toByteArray(2);
}


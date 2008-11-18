// vim:tabstop=4:shiftwidth=4:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2005 Dmitry Poplavsky <dima@thekompany.com>

#ifndef XMLRPC_VARIANT_H
#define XMLRPC_VARIANT_H

#include <QtCore>
#include <QtXml>

namespace  xmlrpc {

/*!
 \class xmlrpc::Variant variant.h
 \brief The xmlrpc::Variant class acts like a union for XML-RPC variables types.

 The xmlrpc::Variant class is the version of QVariant with set of types restricted to
 types supported by XML-RPC specification. For this purpose, the Variant( const QVariant& )
 constructor is declared as private, and xmlrpc::Variant construcors defined for all the
 XML-RPC supported types. This allows to perform the compile time XML-RPC types checks.

 The xmlrpc::Variant class also provides deserialization and serialization from/to QDomElement,
 according to XML-RPC specification, with <value> top DOM element.
 */
class Variant : public QVariant {
public:
    Variant();
	Variant( const Variant& );
	Variant( const QDomElement& );

    /*! Next constructors should be used to build xmlrpc::Variant
        values from users data, to be passed to the xmlrpc::Client::request call*/
    Variant ( uint val );
    Variant ( int val );
    Variant ( bool val );
    Variant ( double val );
    Variant ( const QByteArray & val );
    Variant ( const QString & val );
    Variant ( const QStringList & val );
    Variant ( const QDateTime & val );
    Variant ( const QList<Variant> & val );
    Variant ( const QMap<QString, Variant> & val );

    //! this variable is casted to string XML-RPC type
    Variant ( const QChar & c );

	virtual ~Variant();

    //! serialize Variant data to the QDomDocument, according to XML-RPC specification.
    QDomElement toDomElement( QDomDocument& ) const;

    QString pprint();

private:
    static QString   encodeDateTimeIso8601(QDateTime);
    static QDateTime decodeDateTimeIso8601(QString, bool *ok=0);

    /*! decode the <struct> dom node */
    static Variant decodeStruct( const QDomElement& node );

    /*! decode the <array> dom node */
    static Variant decodeArray( const QDomElement& node );

    /**
     * Variant( const QVariant& ) constructor is declared private
     * to prohibit casting from any QVariant type.
     * This allows to perform the compile time checks
     * for variables types, passed to xml=rpc request.
     */
	Variant( const QVariant& );

    QString pprint( int column );
}; 


} // namespace
#endif // XMLRPC_VARIANT_H


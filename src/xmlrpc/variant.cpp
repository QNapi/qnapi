// vim:tabstop=4:shiftwidth=4:foldmethod=marker:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2005 Dmitry Poplavsky <dima@thekompany.com>

#include <QtXml>
#include <QtCore>

#include "xmlrpc/variant.h"

namespace  xmlrpc {

Variant::Variant()
: QVariant()
{
}

/** Constructs a copy of the variant, p, passed as the argument
 *  to this constructor. */
Variant::Variant( const Variant& val )
: QVariant( val )
{
}

Variant::Variant( const QVariant& val )
: QVariant( val )
{
}


/** Constructs a new variant with an integer value, val. */
Variant::Variant ( int val )
: QVariant( val )
{
}

/** Constructs a new variant with an integer value, val. */
Variant::Variant ( uint val )
: QVariant( val )
{
}

/** Constructs a new variant with a boolean value, val. */
Variant::Variant ( bool val )
: QVariant( val )
{
}


/** Constructs a new variant with a double value, val. */
Variant::Variant ( double val )
: QVariant( val )
{
}

/** Constructs a new variant with an QByteArray value, val. */
Variant::Variant ( const QByteArray & val )
: QVariant( val )
{
}

/** Constructs a new variant with a string value, val. */
Variant::Variant ( const QString & val )
: QVariant( val )
{
}

/** Constructs a new variant with a strings list value, val. */
Variant::Variant ( const QStringList & val )
: QVariant( val )
{
}

/** Constructs a new variant with a char value, val. XmlRPC
string type will be used. */ 
Variant::Variant ( const QChar & c )
: QVariant( QString(c) ) 
{ 
}

/** Constructs a new variant with a date time value, val. */
Variant::Variant ( const QDateTime & val )
: QVariant( val )
{
}

/** Constructs a new variant with a list value, val. */
Variant::Variant ( const QList<Variant> & listVal )
: QVariant()
{
    QList<QVariant> qVal;
    foreach( Variant val, listVal ) {
        qVal.append(val);
    }

    setValue(qVal);
}

/** Constructs a new variant with a map value, val. */
Variant::Variant ( const QMap<QString, Variant> & mapVal  )
: QVariant()
{
    QMap<QString, QVariant> qVal;

    QMapIterator<QString, Variant> i(mapVal);
    while (i.hasNext()) {
        i.next();
        qVal.insert( i.key(), i.value() );
    }

    setValue(qVal);
}


/**
 * Constructs a new variant from XmlRPC xml element node.
 * note name must be "value"
 */
Variant::Variant( const QDomElement& node )
{
    Q_ASSERT( node.nodeName() == "value" );

    if ( node.nodeName() == "value" ) {
        QDomElement data = node.firstChild().toElement();
        if ( data.isNull() ) {
            (*this) = Variant(node.text());
        } else {
            QString tagName = data.tagName();
            QString tagText = data.text();
            bool ok = true;

            if ( tagName == "i4" || tagName == "int" ) {
                (*this) = Variant( tagText.toInt(&ok) );
            } else 
            if ( tagName == "string" ) {
                (*this) = Variant( tagText );
            } else 
            if ( tagName == "double" ) {
                (*this) = Variant( tagText.toDouble(&ok) );
            } else 
            if ( tagName == "base64" ) {
                (*this) = Variant( QByteArray::fromBase64( tagText.toLatin1() ) );
            } else 
            if ( tagName == "boolean" ) {
                bool val = tagText.toInt(&ok) == 1;
                (*this) = Variant( val );
            } else 
            if ( tagName == "dateTime.iso8601" ) {
                (*this) = Variant( decodeDateTimeIso8601(tagText,&ok) );
            } else
            if ( tagName == "struct" ) {
                (*this) = decodeStruct( data );
            } else 
            if ( tagName == "array" ) {
                (*this) = decodeArray( data );
            } else {
                qDebug() << "unexpected node:" << tagName;
                (*this) = Variant(QVariant::Invalid);
            }
        }
    } else {
        qDebug() << "'value' node expected, got"<<node.nodeName();
        (*this) = Variant(QVariant::Invalid);
    }
}

Variant::~Variant()
{
}


QString Variant::encodeDateTimeIso8601(QDateTime date)
{
    return date.toString("yyyyMMddThh:mm:ss");
}

QDateTime Variant::decodeDateTimeIso8601(QString sdate, bool *ok)
{
    // convert from XML-RPC yyyyMMddThh:mm:ss format to Qt's yyyy-MM-ddThh:mm:ss
    sdate = sdate.left(4)+'-'+sdate.mid(4,2)+'-'+sdate.mid(6);
    QDateTime res = QDateTime::fromString( sdate, Qt::ISODate );
    if ( ok ) 
        *ok = res.isValid();

    return res;
}

Variant Variant::decodeStruct( const QDomElement& node )
{
    Q_ASSERT( node.tagName() == "struct" );

    QMap<QString, Variant> res;
    QDomElement member = node.firstChild().toElement();
    while ( !member.isNull() ) {

        Q_ASSERT( member.tagName() == "member" );

        QString name;
        Variant value;
        QDomElement child = member.firstChild().toElement();
        while ( !child.isNull() ) {
            if ( child.tagName() == "name" ) {
                name = child.text();
            }
            if ( child.tagName() == "value" )
                value = Variant( child );

            child = child.nextSibling().toElement();
        }

        if ( !name.isNull() && value.isValid() ) {
            res[name] = value;
        }

        member = member.nextSibling().toElement();

    }

    return Variant(res);
}

Variant Variant::decodeArray( const QDomElement& node )
{
    Q_ASSERT( node.tagName() == "array" );
    Q_ASSERT( node.firstChildElement().tagName() == "data" );
    QList<Variant> res;
    QDomElement data = node.firstChildElement().firstChildElement(); // <array><data><value>


    while ( !data.isNull() ) {

        if ( data.tagName() == "value" ) {
            res.append( Variant( data ) );
        } else 
            qDebug() << "unexpected tag:<"+data.tagName()+">, <value> expected";


        Q_ASSERT( data.tagName() == "value" );

        data = data.nextSibling().toElement();
    }

    return Variant( res );
}

/**
 * Generate QDomElement with XmlRPC presentation of variant
 * value.
 */
QDomElement Variant::toDomElement( QDomDocument& doc ) const
{
    Q_ASSERT( isValid() );

    QDomElement valueEl = doc.createElement("value");
    QDomElement data;

    switch ( type() ) {
    case Int:
    case UInt:
        data = doc.createElement("int");
        data.appendChild( doc.createTextNode(toString()) );
        break;

    case String:
        data = doc.createElement("string");
        data.appendChild( doc.createTextNode(toString()) );
        break;

    case Double:
        data = doc.createElement("double");
        data.appendChild( doc.createTextNode(toString()) );
        break;

    case DateTime:
        data = doc.createElement("dateTime.iso8601");
        data.appendChild( doc.createTextNode( encodeDateTimeIso8601( toDateTime() ) ) );
        break;

    case Bool:
        data = doc.createElement("boolean");
        data.appendChild( doc.createTextNode( toBool() ? "1" : "0"  ) );
        break;

    case ByteArray:
        data = doc.createElement("base64");
        data.appendChild( doc.createTextNode( toByteArray().toBase64() ) );
        break;

    case List:
    case StringList:
        {
        
            data = doc.createElement("array");
            QDomElement arrayData = doc.createElement("data");
            data.appendChild( arrayData );
            QList<QVariant> list = toList();
            QList<QVariant>::iterator it;
            for ( it = list.begin(); it!=list.end(); ++it ) {
                arrayData.appendChild( Variant(*it).toDomElement(doc) );
            }
            break;
        }

    case Map:
        {
            data = doc.createElement("struct");
            QMap<QString,QVariant> map = toMap();
            QMap<QString,QVariant>::Iterator it;
            for ( it = map.begin(); it!=map.end(); ++it ) {
                QDomElement member = doc.createElement("member");
                data.appendChild( member );
    
                QDomElement name = doc.createElement("name");
                name.appendChild( doc.createTextNode( it.key() ) );
                QDomElement value = Variant(it.value()).toDomElement(doc);
    
                member.appendChild( name );
                member.appendChild( value );
            }
            break;
        }
    default: 
        {
            // this should never be called
            qWarning("type %s is not supported", typeName() );
            Q_ASSERT_X( false, "Variant::toDomElement", "type not supported" );
        }
     };

     if ( !data.isNull() ) {
         valueEl.appendChild( data );
     }

     return valueEl;

}


/**
 * Return  human readable presentation of xmlrpc::Variant value
 * in JSON like format.
 */
QString Variant::pprint()
{
    return pprint(0);
}

QString Variant::pprint( int column )
{
    column = qMin( column, 48 );
    
    QStringList items;

    switch ( type() ) {
    case Int:
    case UInt:
        items << QString::number( toInt() );
        break;

    case String:
        items << QString("\"%1\"").arg( toString() );
        break;

    case Double:
        items << QString::number( toDouble() );
        break;

    case DateTime:
        items << QString("'%1'").arg( toDateTime().toString() );
        break;

    case Bool:
        items << ( toBool() ? "true" : "false" );
        break;

    case ByteArray:
        {
            QString data = toByteArray().toBase64();
            if ( data.length() > 128 ) {
                data = data.left(64)+"...";
            }
            items << data;
            break;
        }

    case List:
    case StringList:
        {
            items << "[ ";
            QListIterator<QVariant> it( toList() );
            while( it.hasNext() ) {
                QString val = Variant(it.next()).pprint( column+2 );
                if ( it.hasNext() )
                    val += ","; 

                items << val+" ";
            }

            items << "]";
            break;
        }

    case Map:
        {
            items << "{ ";
            QMapIterator<QString,QVariant> it(toMap());
            while( it.hasNext() ) {
                it.next();
                QString val = Variant(it.value()).pprint(column+2);
                if ( it.hasNext() )
                    val += ","; 

                items << it.key()+"="+val+" ";
            }
            items << "}";
            break;
        }
    default:
        items << "unexpected element " + toString(); 
    };

    QString res;

    int columnPos = column;
    foreach ( QString item, items ) {
        if ( columnPos + item.length() > 128 ) {
            // move to the new line
            res += "\n";
            res += QString( column, ' ' );
            res += item;
            columnPos = column + item.length();
        } else {
            res += item;
            columnPos += item.length();
        }
    }

    return res;
}

} 



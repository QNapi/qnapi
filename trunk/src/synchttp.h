/***************************************************************************
 *   Copyright (C) 2005 by Iulian M                                        *
 *   eti@erata.net                                                         *
 *   syncRequest added by Krzemin <pkrzemin@o2.pl>                         *
 ***************************************************************************/
#ifndef ETKSYNCHTTP_H
#define ETKSYNCHTTP_H

#include <QHttp>
#include <QEventLoop>
#include <QBuffer>

/**
 * Provide a synchronous api over QHttp
 * Uses a QEventLoop to block until the request is completed
 * @author Iulian M <eti@erata.net>
*/
class SyncHTTP: public QHttp
{
	Q_OBJECT
	public:
		/// structors
		SyncHTTP( QObject * parent = 0 )
		:QHttp(parent),requestID(-1),status(false){}

		SyncHTTP( const QString & hostName, quint16 port = 80, QObject * parent = 0 )
		:QHttp(hostName,port,parent),requestID(-1),status(false){}

		virtual ~SyncHTTP(){}

		/// send GET request and wait until finished
		bool syncGet ( const QString & path, QIODevice * to = 0 )
		{
			///connect the requestFinished signal to our finished slot
			connect(this,SIGNAL(requestFinished(int,bool)),SLOT(finished(int,bool)));
			/// start the request and store the requestID
			requestID = get(path, to);
			/// block until the request is finished
			loop.exec();
			/// return the request status
			return status;
		}

		/// send POST request and wait until finished
		bool syncPost ( const QString & path, QIODevice * data, QIODevice * to = 0 )
		{
			///connect the requestFinished signal to our finished slot
			connect(this,SIGNAL(requestFinished(int,bool)),SLOT(finished(int,bool)));
			/// start the request and store the requestID
			requestID = post(path, data , to);
			/// block until the request is finished
			loop.exec();
			/// return the request status
			return status;
		}

		bool syncPost ( const QString & path, const QByteArray& data, QIODevice * to = 0 )
		{
			/// create io device from QByteArray
			QBuffer buffer;
			buffer.setData(data);
			return syncPost(path,&buffer,to);
		}

		bool syncRequest ( const QHttpRequestHeader & header, QIODevice * data, QIODevice * to = 0 )
		{
			connect(this,SIGNAL(requestFinished(int,bool)),SLOT(finished(int,bool)));
			requestID = request(header, data , to);
			loop.exec();
			return status;
		}

		bool syncRequest ( const QHttpRequestHeader & header, const QByteArray & data, QIODevice * to = 0 )
		{
			QBuffer buffer;
			buffer.setData(data);
			return syncRequest(header, &buffer, to);
		}

	protected slots:
		virtual void finished(int idx, bool err)
		{
			/// check to see if it's the request we made
			if(idx!=requestID)
				return;
			/// set status of the request
			status = !err;
			/// end the loop
			loop.exit();
		}

	private:
		/// id of current request
		int requestID;
		/// error status of current request
		bool status;
		/// event loop used to block until request finished
		QEventLoop loop;
};

#endif

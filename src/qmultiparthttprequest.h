/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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

#ifndef __QMULTIPARTHTTPREQUEST__H__
#define __QMULTIPARTHTTPREQUEST__H__

#include <QString>
#include <QVector>
#include <QTime>

class QMultipartHttpRequest
{
	public:
		void addBoundary();
		void addEndingBoundary();
		void addContentDisposition(const QString & contentDisposition);
		void addContentType(const QString & contentType);
		void addData(const QByteArray & data);
		void addData(const QString & data);

		QByteArray & requestStream();
		QString & boundaryTxt();

	private:
		struct requestElement
		{
			enum elementType
			{
				ET_CONTENT_DISPOSITION,
				ET_CONTENT_TYPE,
				ET_DATA,
				ET_BOUNDARY,
				ET_ENDING_BOUNDARY,
			};

			elementType type;
			QByteArray elementData;
		};

		void generateBoundary();
		void addElement(requestElement::elementType type, const QByteArray & data = "");

		QVector<requestElement> elements;
		QString boundary;
		QByteArray buffer;
};

#endif

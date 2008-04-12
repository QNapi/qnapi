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

#include "multipartdata.h"

void MultipartData::addBoundary()
{
	addElement(requestElement::ET_BOUNDARY);
}

void MultipartData::addEndingBoundary()
{
	addElement(requestElement::ET_ENDING_BOUNDARY);
}

void MultipartData::addContentDisposition(const QString & contentDisposition)
{
	addElement(requestElement::ET_CONTENT_DISPOSITION, contentDisposition.toAscii());
}

void MultipartData::addContentType(const QString & contentType)
{
	addElement(requestElement::ET_CONTENT_TYPE, contentType.toAscii());
}

void MultipartData::addData(const QByteArray & data)
{
	addElement(requestElement::ET_DATA, data);
}

void MultipartData::addData(const QString & data)
{
	addElement(requestElement::ET_DATA, data.toAscii());
}

QByteArray & MultipartData::requestStream()
{
	generateBoundary();

	QVector<requestElement>::iterator el;
	const char *endl = "\r\n";
	buffer.clear();

	for(el = elements.begin(); el != elements.end(); el++)
	{
		switch((*el).type)
		{
			case requestElement::ET_CONTENT_DISPOSITION:
				buffer += QString("Content-Disposition: form-data; ") + (*el).elementData + endl;
			break;

			case requestElement::ET_CONTENT_TYPE:
				buffer += QString("Content-Type: ") + (*el).elementData + endl;
			break;

			case requestElement::ET_BOUNDARY:
				buffer += QString("--") + boundary + endl;
			break;
			
			case requestElement::ET_ENDING_BOUNDARY:
				buffer += endl + QString("--") + boundary + QString("--") + endl;
			break;

			case requestElement::ET_DATA:
				buffer += endl + (*el).elementData + endl;
			break;
		}
	}

	return buffer;
}

QString & MultipartData::boundaryTxt()
{
	return boundary;
}

void MultipartData::generateBoundary()
{
	bool binarySafe;
	QVector<requestElement>::iterator el;

	QTime midnight(0, 0, 0);
	qsrand(midnight.secsTo(QTime::currentTime()));

	do {
		boundary = QString::number(qrand());

		binarySafe = true;

		for(el = elements.begin(); el != elements.end(); el++)
		{
			if((*el).elementData.indexOf(boundary) >= 0)
			{
				binarySafe = false;
				break;
			}
		}
	} while (!binarySafe);
}

void MultipartData::addElement(requestElement::elementType type, const QByteArray & data)
{
	requestElement el;
	el.type = type;
	el.elementData = data;
	elements.push_back(el);
}

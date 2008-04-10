/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef __MULTIPARTDATA__H__
#define __MULTIPARTDATA__H__

#include <QString>
#include <QVector>
#include <QTime>

class MultipartData
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

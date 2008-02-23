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

#ifndef ENCODINGUTILS_H
#define ENCODINGUTILS_H

#include <QString>
#include <QStringList>
#include <QByteArray>

class EncodingUtils
{
public:
    EncodingUtils();

    QString replaceDiacriticsWithASCII(const QString & str);
    QString detectBufferEncoding(const QByteArray & buffer);

private:
    QString diacritics;
    QStringList replacements;
    QStringList codecs;
};

#endif // ENCODINGUTILS_H

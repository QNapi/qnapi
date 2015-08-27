/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "encodingutils.h"

#include <QTextStream>

EncodingUtils::EncodingUtils()
{
    diacritics = QString::fromUtf8("ąćęłśżźĄĆĘŁŚŻŹŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
    replacements <<"a"<<"c"<<"e"<<"l"<<"s"<<"z"<<"z"<<"A"<<"C"<<"E"<<"L"<<"S"<<"Z"<<"Z"<<"S"<<"OE"
        <<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"
        <<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"
        <<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"
        <<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
    codecs << "windows-1250" << "windows-1257" << "ISO-8859-2"
            << "ISO-8859-13" << "ISO-8859-16" << "UTF-8";
}

QString EncodingUtils::replaceDiacriticsWithASCII(const QString & str) {
    QString output;
    for (int i = 0; i < str.length(); i++) {
        QChar c = str[i];
        int dIndex = diacritics.indexOf(c);
        if (dIndex < 0) {
            output.append(c);
        } else {
            QString replacement = replacements[dIndex];
            output.append(replacement);
        }
    }
    return output;
}

QString EncodingUtils::detectBufferEncoding(const QByteArray & buffer) {
    QString from;

    int bestMatch = 0;

    foreach(QString codec, codecs)
    {
        QTextStream ts(buffer);
        ts.setCodec(qPrintable(codec));
        QString encodedData = ts.readAll();
        QStringList chars = QString::fromUtf8("ą/ś/ż/ć/ń/ł/ó/ę").split("/");

        int i;
        for (i = 0; i < chars.count(); i++)
        {
            if(!encodedData.contains(chars[i], Qt::CaseInsensitive))
                break;

            if(i + 1 > bestMatch) {
                bestMatch = i + 1;
                from = codec;
            }
        }
    }

    return from;
}

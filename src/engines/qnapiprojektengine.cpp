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

#include "qnapiprojektengine.h"
#include "forms/frmnapiprojektconfig.h"
#include "libmediainfomovieinfoparser.h"
#include "qmultiparthttprequest.h"
#include "synchttp.h"
#include "movieinfo.h"
#include "qnapilanguage.h"

#include <cmath>

#include <QCryptographicHash>
#include <QUrl>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>

#ifdef Q_OS_WIN // for SetFileAttributes
#include <qt_windows.h>
#endif

// This sadly has to be added in order to build under VS >= 2005
#ifdef _MSC_VER
    // from Visual Studio 2005 onwards
    // https://msdn.microsoft.com/en-us/library/2ts7cx93%28v=vs.80%29.aspx
    #include <stdio.h>
    #define snprintf _snprintf_s
#endif

// konstruktor klasy
QNapiProjektEngine::QNapiProjektEngine()
{
    p7zipPath = GlobalConfig().p7zipPath();
    nick = GlobalConfig().nick(engineName());
    pass = GlobalConfig().pass(engineName());
    noBackup = GlobalConfig().noBackup();
}

// destruktor klasy
QNapiProjektEngine::~QNapiProjektEngine()
{
    cleanup();
}


// zwraca nazwe modulu
QString QNapiProjektEngine::engineName()
{
    return "NapiProjekt";
}

// zwraca informacje nt. modulu
QString QNapiProjektEngine::engineInfo()
{
    return "Moduł pobierania napisów z bazy <b>www.napiprojekt.pl</b>";
}

// zwraca ikone w formacie XMP
QIcon QNapiProjektEngine::engineIcon()
{
    static const char * const icon[] = {
        "16 16 5 1",
        "   c #FFFFFF",
        ".  c #000080",
        "+  c #0052CC",
        "@  c #005CE6",
        "#  c #BFD9FF",
        "           ...  ",
        "         ... .  ",
        "       ... ...  ",
        "     ... ....   ",
        "   ... ....     ",
        " ... ....       ",
        ".. ....         ",
        "....+           ",
        " ...............",
        " . . . . . . . .",
        " ...............",
        " .@@####@####@@.",
        " .@@#@@#@#@@#@@.",
        " .@@#@@#@####@@.",
        " .@@#@@#@#@@@@@.",
        " ..............."};
    return QIcon(QPixmap(icon));
}

// zwraca czy silnik jest konfigurowalny
bool QNapiProjektEngine::isConfigurable()
{
    return true;
}

// wywoluje okienko konfiguracji
void QNapiProjektEngine::configure(QWidget * parent)
{
    frmNapiProjektConfig config(parent);
    config.exec();
}

// oblicza sume kontrolna dla pliku filmowego (md5 z pierwszych 10MB pliku)
QString QNapiProjektEngine::checksum(QString filename)
{
    if(filename.isEmpty())
        filename = movie;
    return (checkSum = checksum(filename, true));
}


bool QNapiProjektEngine::lookForSubtitles(QString lang)
{
    Maybe<QString> tmpPackedFileOpt = downloadByLangAndChecksum(lang, checkSum);

    if(!tmpPackedFileOpt)
    {
        return false;
    }

    QString tmpPackedFile = tmpPackedFileOpt.value();

    subtitlesList << QNapiSubtitleInfo( lang,
                                        engineName(),
                                        tmpPackedFile,
                                        QFileInfo(movie).completeBaseName(),
                                        "",
                                        "txt",
                                        SUBTITLE_UNKNOWN);
    return true;
}

QList<QNapiSubtitleInfo> QNapiProjektEngine::listSubtitles()
{
    return subtitlesList;
}

// Probuje pobrac napisy do filmu z serwera NAPI
bool QNapiProjektEngine::download(QUuid id)
{
    Maybe<QNapiSubtitleInfo> ms = resolveById(id);

    return ms && QFile::exists(ms.value().sourceLocation);
}

Maybe<QString> QNapiProjektEngine::downloadByLangAndChecksum(QString lang, QString checksum) const
{
    if(checksum.isEmpty())
        return nothing();

    QString urlTxt = napiDownloadUrlTpl.arg(npLangWrapper(lang))
                                       .arg(checksum)
                                       .arg(npFDigest(checksum))
                                       .arg(nick)
                                       .arg(pass)
                                       .arg(napiOS());

    QUrl url(urlTxt);

    SyncHTTP http;
    QNetworkReply *reply = http.syncGet(QNetworkRequest(url));

    if(reply->error() != QNetworkReply::NoError)
        return nothing();

    QByteArray buffer = reply->readAll();

    if(buffer.indexOf("NPc") == 0)
        return nothing();

    QString tmpPackedFile = generateTmpPath();

    QFile file(tmpPackedFile);
    if(file.exists()) file.remove();
    if(!file.open(QIODevice::WriteOnly))
        return nothing();

    int r = file.write(buffer);
    file.close();

    if(r < 0)
        return nothing();

    return just(tmpPackedFile);
}

// Probuje rozpakowac napisy do filmu
bool QNapiProjektEngine::unpack(QUuid id)
{
    Maybe<QNapiSubtitleInfo> ms = resolveById(id);
    if(!ms) return false;

    if(!QFile::exists(movie)) return false;
    subtitlesTmp = tmpPath + "/" + checkSum + ".txt";

    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);

    QStringList args;
    args << "e" << "-y" << ("-p" + napiZipPassword) << ("-o" + tmpPath) << ms.value().sourceLocation;

    QProcess p7zip;
    p7zip.start(p7zipPath, args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;

    return QFile::exists(subtitlesTmp);
}

void QNapiProjektEngine::cleanup()
{
    clearSubtitlesList();
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);
}

// Sprawdza uzytkownika w bazie
bool QNapiProjektEngine::checkUser(const QString & nick, const QString & pass)
{
    SyncHTTP http;
    QString urlTxt = napiCheckUserUrlTpl.arg(nick).arg(pass);

    QUrl url(urlTxt);
    QNetworkReply *reply = http.syncGet(QNetworkRequest(url));
    if(reply->error() != QNetworkReply::NoError)
        return false;

    QString buffer = reply->readAll();
    if(buffer.indexOf("ok") == 0) return true;
    return false;
}

// oblicza sume kontrolna dla pliku filmowego (md5 z pierwszych 10MB pliku)
QString QNapiProjektEngine::checksum(QString filename, bool limit10M)
{
    QFile file(filename);
    QByteArray fileArray;

    if(!file.open(QIODevice::ReadOnly))
        return QString("");

    fileArray = limit10M ? file.read(NAPI_10MB) : file.readAll();
    file.close();

    QByteArray b = QCryptographicHash::hash(fileArray, QCryptographicHash::Md5);
    fileArray.clear();

    QString out;
    char next[3];

    for(int i = 0; i < 16; i++)
    {
        snprintf(next, 3, "%.2x", (unsigned char)b[i]);
        out += next;
    }
    
    checkSum = out;
    return checkSum;
}

// Tajemnicza funkcja f()
QString QNapiProjektEngine::npFDigest(const QString & input) const
{
    if(input.size() != 32) return "";

    int idx[] = {0xe, 0x3, 0x6, 0x8, 0x2},
        mul[] = {2, 2, 5, 4, 3},
        add[] = {0x0, 0xd, 0x10, 0xb, 0x5},
        a, m, i, t, v;

    QString b;
    char tmp[2] = { 0, 0 };

    for(int j = 0; j <= 4; j++)
    {
        a = add[j];
        m = mul[j];
        i = idx[j];

        tmp[0] = input[i].toLatin1();
        t = a + (int)(strtol(tmp, NULL, 16));
        v = (int)(strtol(input.mid(t, 2).toLocal8Bit(), NULL, 16));

        snprintf(tmp, 2, "%x", (v * m) % 0x10);
        b += tmp;
    }

    return b;
}

QString QNapiProjektEngine::npLangWrapper(QString lang) const
{
    lang = QNapiLanguage(lang).toTwoLetter().toUpper();

    if(lang == "EN")
        lang = "ENG";

    return lang;
}

QString QNapiProjektEngine::napiOS() const
{
#if defined(Q_OS_WIN)
    return "Windows";
#elif defined(Q_OS_MAC)
    return "Mac OS X";
#else
    return "Linux/UNIX";
#endif
}

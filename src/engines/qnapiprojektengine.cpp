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
#include <QNetworkRequest>
#include <QNetworkReply>
#include "libmediainfomovieinfoparser.h"

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
QNapiProjektEngine::QNapiProjektEngine(const QString & movieFile, const QString & subtitlesFile)
    : QNapiAbstractEngine(movieFile, subtitlesFile)
{
    p7zipPath = GlobalConfig().p7zipPath();
    nick = GlobalConfig().nick(engineName());
    pass = GlobalConfig().pass(engineName());
    noBackup = GlobalConfig().noBackup();
    tmpPackedFile =  QString("%1/%2").arg(tmpPath).arg(generateTmpFileName());
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
    return "Modul pobierania napisów z bazy <b>www.napiprojekt.pl</b>";
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
    if(checkSum.isEmpty()) return false;

    subtitlesList.clear();

    SyncHTTP http;
    QString urlTxt = napiDownloadUrlTpl.arg(npLangWrapper(lang))
                                        .arg(checkSum)
                                        .arg(npFDigest(checkSum))
                                        .arg(nick)
                                        .arg(pass)
#ifdef Q_OS_WIN
                                        .arg("Windows");
#elif defined(Q_OS_MAC)
                                        .arg("Mac OS X");
#else
                                        .arg("Linux/UNIX");
#endif

    QUrl url(urlTxt);

    QNetworkReply *reply = http.syncGet(QNetworkRequest(url));

    if(reply->error() != QNetworkReply::NoError)
        return false;

    QByteArray buffer = reply->readAll();

    if(buffer.indexOf("NPc") == 0)
        return false;

    QFile file(tmpPackedFile);
    if(file.exists()) file.remove();
    if(!file.open(QIODevice::WriteOnly))
        return false;

    int r = file.write(buffer);
    file.close();
    
    if(!r) return false;
    
    subtitlesList << QNapiSubtitleInfo( lang,
                                        engineName(),
                                        urlTxt,
                                        QFileInfo(movie).completeBaseName(),
                                        "",
                                        "txt",
                                        SUBTITLE_UNKNOWN);

    return (subtitlesList.size() > 0);
}

QList<QNapiSubtitleInfo> QNapiProjektEngine::listSubtitles()
{
    return subtitlesList;
}

// Probuje pobrac napisy do filmu z serwera NAPI
bool QNapiProjektEngine::download(int idx)
{
    Q_UNUSED(idx)
    
    return (subtitlesList.size() > 0);
}

// Probuje rozpakowac napisy do filmu
bool QNapiProjektEngine::unpack()
{
    if(!QFile::exists(tmpPackedFile)) return false;
    if(!QFile::exists(movie)) return false;
    subtitlesTmp = tmpPath + "/" + checkSum + ".txt";

    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);

    QStringList args;
    args << "e" << "-y" << ("-p" + napiZipPassword) << ("-o" + tmpPath) << tmpPackedFile;

    QProcess p7zip;
    p7zip.start(p7zipPath, args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;

    return QFile::exists(subtitlesTmp);
}

void QNapiProjektEngine::cleanup()
{
    if(QFile::exists(tmpPackedFile))
        QFile::remove(tmpPackedFile);
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

// Wrzuca napisy do bazy NAPI
QNapiProjektEngine::UploadResult
    QNapiProjektEngine::uploadSubtitles(const QString & language, const QString & nick,
                                        const QString & pass, bool correct, const QString & comment)
{
    if(!QFile::exists(movie) || !QFile::exists(subtitles))
        return NAPI_FAIL;

    LibmediainfoMovieInfoParser mip;
    MovieInfo movieInfo = mip.parseFile(movie);

    unsigned long movie_size = QFileInfo(movie).size();
    QString movie_md5 = checksum(movie);
    QString subtitles_md5 = checksum(subtitles, false);

    QString newSubtitlesName = tmpPath + "/" + movie_md5 + ".txt";
    if(QFile::exists(newSubtitlesName)) QFile::remove(newSubtitlesName);
    if(!QFile::copy(subtitles, newSubtitlesName))
        return NAPI_FAIL;

    QString zipFileName = QFileInfo(newSubtitlesName).path() + "/"
                            + QFileInfo(newSubtitlesName).completeBaseName() + ".zip";

    if(QFile::exists(zipFileName))
        QFile::remove(zipFileName);

    QStringList args;
    args << "a" << "-t7z" << "-bd" << "-y";
#ifndef Q_OS_WIN
    args << "-l";
#endif
    args << zipFileName << (QString("-p")+napiZipPassword) << newSubtitlesName;

    QProcess p7zip;
    p7zip.start(p7zipPath, args);

    if(!p7zip.waitForFinished())
        return NAPI_FAIL;

    QFile::remove(newSubtitlesName);

    if(!QFile::exists(zipFileName))
        return NAPI_FAIL;

    // Przygotowujemy dane do zapytania POST
    QMultipartHttpRequest postData;

    postData.addBoundary();
    postData.addContentDisposition("name=\"kmt\"");
    postData.addData(comment);

    postData.addBoundary();
    postData.addContentDisposition("name=\"poprawka\"");
    postData.addData(QString(correct ? "true" : "false"));

    postData.addBoundary();
    postData.addContentDisposition("name=\"v\"");
    postData.addData(QString("other"));

    postData.addBoundary();
    postData.addContentDisposition("name=\"l\"");
    postData.addData(npLangWrapper(language));

    postData.addBoundary();
    postData.addContentDisposition("name=\"t\"");
    postData.addData(npFDigest(movie_md5));

    postData.addBoundary();
    postData.addContentDisposition("name=\"m_filename\"");
    postData.addData(QFileInfo(movie).fileName());

    postData.addBoundary();
    postData.addContentDisposition("name=\"nick\"");
    postData.addData(nick);

    postData.addBoundary();
    postData.addContentDisposition("name=\"pass\"");
    postData.addData(pass);

    postData.addBoundary();
    postData.addContentDisposition("name=\"s_hash\"");
    postData.addData(subtitles_md5);

    postData.addBoundary();
    postData.addContentDisposition("name=\"MAX_FILE_SIZE\"");
    postData.addData(QString("512000"));

    postData.addBoundary();
    postData.addContentDisposition("name=\"plik\"; filename=\"" +
        QFileInfo(zipFileName).fileName() + "\"");
    postData.addContentType("subtitles/zip");

    QFile fZip(zipFileName);
    if(!fZip.open(QIODevice::ReadOnly))
        return NAPI_FAIL;

    postData.addData(fZip.readAll());
    postData.addEndingBoundary();

    fZip.close();
    QFile::remove(zipFileName);

    QByteArray data = postData.requestStream();

    QString movie_fps = QString::number((int)ceil(movieInfo.frameRate * 100)).replace(".", ",");

    QString urlTxt;
    
    if(!movieInfo.isFilled)
    {
        urlTxt = napiUploadUrlSimpleTpl.arg(movie_md5).arg(movie_size);
    }
    else
    {
        urlTxt = napiUploadUrlTpl.arg(movieInfo.durationSecs).arg(movieInfo.width)
                                    .arg(movieInfo.height).arg(movie_fps)
                                    .arg(movie_md5).arg(movie_size);
    }

    QUrl url(urlTxt);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QString("QNapi ") + QNAPI_VERSION);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + postData.boundaryTxt());

    SyncHTTP http;
    QNetworkReply *reply = http.syncPost(req, data);
    if(reply->error() != QNetworkReply::NoError)
        return NAPI_FAIL;

    QString response = reply->readAll();

    if(response.indexOf("NPc0") == 0)
        return NAPI_ADDED_NEW;
    if((response.indexOf("NPc2") == 0) || (response.indexOf("NPc3") == 0))
        return NAPI_OK;
    return NAPI_UNKNOWN;
}

// Wysyla blad o niepasujacyh napisach
QNapiProjektEngine::ReportResult
    QNapiProjektEngine::reportBad(const QString & language, const QString & nick, const QString & pass,
                                    const QString & comment, QString *response)
{
    QFileInfo fi(movie);
    subtitles = fi.path() + "/" + fi.completeBaseName() + ".txt";

    if(!QFile::exists(subtitles))
        return NAPI_NO_SUBTITLES;

    checksum();

    QMultipartHttpRequest postData;

    postData.addBoundary();
    postData.addContentDisposition("name=\"nick\"");
    postData.addData(nick);

    postData.addBoundary();
    postData.addContentDisposition("name=\"pass\"");
    postData.addData(pass);

    postData.addBoundary();
    postData.addContentDisposition("name=\"l\"");
    postData.addData(npLangWrapper(language));

    postData.addBoundary();
    postData.addContentDisposition("name=\"md5\"");
    postData.addData(checkSum);

    postData.addBoundary();
    postData.addContentDisposition("name=\"kmt\"");
    postData.addData(comment);
    postData.addEndingBoundary();

    QByteArray data = postData.requestStream();

    QUrl url(napiReportBadUrlTpl);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QString("QNapi ") + QNAPI_VERSION);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + postData.boundaryTxt());

    SyncHTTP http;
    QNetworkReply *reply = http.syncPost(req, data);
    if(reply->error() != QNetworkReply::NoError)
        return NAPI_NOT_REPORTED;

    *response = QTextCodec::codecForName("windows-1250")->toUnicode(reply->readAll());

    return NAPI_REPORTED;
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
QString QNapiProjektEngine::npFDigest(const QString & input)
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

QString QNapiProjektEngine::npLangWrapper(QString lang)
{
    lang = QNapiLanguage(lang).toTwoLetter().toUpper();

    if(lang == "EN")
        lang = "ENG";

    return lang;
}

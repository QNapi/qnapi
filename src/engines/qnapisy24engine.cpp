/*****************************************************************************
** QNapi
** Copyright (C) 2015 fenuks
** Copyright (C) 2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "qnapisy24engine.h"
#include "forms/frmnapisy24config.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

QNapisy24Engine::QNapisy24Engine()
{
    p7zipPath = GlobalConfig().p7zipPath();
    noBackup = GlobalConfig().noBackup();
}

QNapisy24Engine::~QNapisy24Engine()
{
    cleanup();
}

QString QNapisy24Engine::engineName()
{
    return "Napisy24";
}

QString QNapisy24Engine::engineInfo()
{
    return "Moduł pobierania napisów z bazy <b>www.napisy24.pl</b><br />";
}

QIcon QNapisy24Engine::engineIcon()
{
    static const char * icon[] = {
    "16 16 256 2",
    "  	c #D6D9DB",
    ". 	c #F5F6F7",
    "+ 	c #E4E6E9",
    "@ 	c #F0F1F2",
    "# 	c #2673BC",
    "$ 	c #2869B7",
    "% 	c #6B9ACC",
    "& 	c #E2E5E8",
    "* 	c #75A7D3",
    "= 	c #F1C53B",
    "- 	c #D7B04F",
    "; 	c #EDEEF0",
    "> 	c #B2C5DB",
    ", 	c #EEC750",
    "' 	c #EEF0F2",
    ") 	c #DFE2E5",
    "! 	c #1B62B2",
    "~ 	c #F1F3F4",
    "{ 	c #E5E8EC",
    "] 	c #E8A81C",
    "^ 	c #E7EAED",
    "/ 	c #F2F4F6",
    "( 	c #EDCA69",
    "_ 	c #3786C9",
    ": 	c #DEE1E4",
    "< 	c #287AC1",
    "[ 	c #4B76B4",
    "} 	c #4685C4",
    "| 	c #E1E4E7",
    "1 	c #1559AD",
    "2 	c #DCE3EB",
    "3 	c #EDBC33",
    "4 	c #5F83BA",
    "5 	c #A8C7E2",
    "6 	c #0954AA",
    "7 	c #FAFBFB",
    "8 	c #EAEDF0",
    "9 	c #EEE1B9",
    "0 	c #B6CBE1",
    "a 	c #F9FAFB",
    "b 	c #F8F8F9",
    "c 	c #F8F9FA",
    "d 	c #E9E5D4",
    "e 	c #DAAA31",
    "f 	c #E8DBB9",
    "g 	c #E9ECEE",
    "h 	c #07469D",
    "i 	c #3575BA",
    "j 	c #E6E8EA",
    "k 	c #155CB0",
    "l 	c #87B1D8",
    "m 	c #ECEDEE",
    "n 	c #D8E5F1",
    "o 	c #CBD0D6",
    "p 	c #DADDE0",
    "q 	c #F7F8F9",
    "r 	c #EBD69A",
    "s 	c #EDB122",
    "t 	c #A8BBD7",
    "u 	c #1867AB",
    "v 	c #E8D8AC",
    "w 	c #EFC137",
    "x 	c #4498D3",
    "y 	c #9BBDDB",
    "z 	c #F4F5F6",
    "A 	c #ECE3C7",
    "B 	c #195DAE",
    "C 	c #1E6BBA",
    "D 	c #EDBA2C",
    "E 	c #1660B2",
    "F 	c #1356AA",
    "G 	c #E6EAEC",
    "H 	c #E3EAF3",
    "I 	c #E8E9EB",
    "J 	c #064CA3",
    "K 	c #CDDBE9",
    "L 	c #74B2DF",
    "M 	c #C4D3E3",
    "N 	c #1D71B4",
    "O 	c #F4F4F4",
    "P 	c #BAD1E6",
    "Q 	c #81ABD4",
    "R 	c #E8EBED",
    "S 	c #3C7EC2",
    "T 	c #F0BB29",
    "U 	c #E4E7EA",
    "V 	c #D5E1EE",
    "W 	c #F6F6F6",
    "X 	c #D1DEEA",
    "Y 	c #F1F1EB",
    "Z 	c #548AC3",
    "` 	c #F3F4F6",
    " .	c #EAAD20",
    "..	c #5F91C7",
    "+.	c #EFBE32",
    "@.	c #F0DC9C",
    "#.	c #CFD5DB",
    "$.	c #5BA3D5",
    "%.	c #DDDFE3",
    "&.	c #95C4E5",
    "*.	c #EDD080",
    "=.	c #E8EAEE",
    "-.	c #D49912",
    ";.	c #FAFBFC",
    ">.	c #1F69B4",
    ",.	c #F1EAD3",
    "'.	c #BFCBDC",
    ").	c #EEF2F5",
    "!.	c #ECBE39",
    "~.	c #EFEEE9",
    "{.	c #F1E9CA",
    "].	c #E4E8ED",
    "^.	c #E9A209",
    "/.	c #2B5CA5",
    "(.	c #D3DBE5",
    "_.	c #ECB529",
    ":.	c #4C8FCB",
    "<.	c #3995D7",
    "[.	c #E4E5E7",
    "}.	c #EDE9DF",
    "|.	c #1052A5",
    "1.	c #DCA41C",
    "2.	c #EDB824",
    "3.	c #95BBDA",
    "4.	c #539BD3",
    "5.	c #033D97",
    "6.	c #CBD5E2",
    "7.	c #2E7FC7",
    "8.	c #91B6D9",
    "9.	c #C3D5E7",
    "0.	c #2E7EBF",
    "a.	c #83BAE2",
    "b.	c #CCD8E7",
    "c.	c #2864B2",
    "d.	c #91B2D6",
    "e.	c #E6CD8A",
    "f.	c #90A7CD",
    "g.	c #E2E7F0",
    "h.	c #9FB4D1",
    "i.	c #E4DECA",
    "j.	c #F5F4F1",
    "k.	c #E9EAEB",
    "l.	c #EAECED",
    "m.	c #0D63B4",
    "n.	c #ECAF1B",
    "o.	c #C3D9EC",
    "p.	c #8EADD3",
    "q.	c #306CB4",
    "r.	c #3D8FCC",
    "s.	c #0D5BAF",
    "t.	c #E1E8EE",
    "u.	c #DEE5ED",
    "v.	c #E9EEF3",
    "w.	c #398ACD",
    "x.	c #0B60A8",
    "y.	c #A0C0DE",
    "z.	c #E1C26A",
    "A.	c #C5D1DF",
    "B.	c #3E67A9",
    "C.	c #F2F2F3",
    "D.	c #E5E7E9",
    "E.	c #1B66B6",
    "F.	c #EEEEEB",
    "G.	c #EEB632",
    "H.	c #B9DAF1",
    "I.	c #EBF0F6",
    "J.	c #7CA1CC",
    "K.	c #E7E8EB",
    "L.	c #BFCEE1",
    "M.	c #104CA0",
    "N.	c #7B96C3",
    "O.	c #1C52A4",
    "P.	c #ECD38D",
    "Q.	c #F2F3F3",
    "R.	c #F1C131",
    "S.	c #E4C87A",
    "T.	c #CEDFEC",
    "U.	c #F4F6F4",
    "V.	c #F6F6F4",
    "W.	c #60ABDF",
    "X.	c #E7B423",
    "Y.	c #176ABB",
    "Z.	c #1664B6",
    "`.	c #EFF5F9",
    " +	c #165FA0",
    ".+	c #F6F4F5",
    "++	c #F8F8F6",
    "@+	c #116CB2",
    "#+	c #C7E1F3",
    "$+	c #9CB8D8",
    "%+	c #F0DEA6",
    "&+	c #E0E1E2",
    "*+	c #E5EAEF",
    "=+	c #E8BD45",
    "-+	c #F3F3F2",
    ";+	c #E6ECF1",
    ">+	c #E6D097",
    ",+	c #1A57A9",
    "'+	c #E4E8F0",
    ")+	c #2170BD",
    "!+	c #E4E8E9",
    "~+	c #F6F7F8",
    "{+	c #1E4C9C",
    "]+	c #F8F6F5",
    "^+	c #EEEFEF",
    "/+	c #E5E7EB",
    "(+	c #D9E1E9",
    "_+	c #ECEFF3",
    ":+	c #F1F0F1",
    "<+	c #D3A027",
    "[+	c #D7DFE8",
    "}+	c #308BD2",
    "|+	c #C8CCCF",
    "1+	c #EEB83B",
    "2+	c #EBEBE9",
    "3+	c #E6E9ED",
    "4+	c #EBF0F3",
    "5+	c #E2E8EB",
    "6+	c #C8D9E8",
    "7+	c #C7D7E8",
    "8+	c #DEB43B",
    "9+	c #E5A008",
    "0+	c #FDFDFD",
    "a+	c #FCFCFC",
    "b+	c #FDFEFE",
    "c+	c #FFFFFF",
    "d+	c #FEFFFF",
    "e+	c #FEFEFF",
    "f+	c #FBFCFC",
    "g+	c #FCFDFD",
    "h+	c #FEFEFE",
    "i+	c #FDFDFE",
    "j+	c #F5F5F6",
    "k+	c #EBEEF0",
    "l+	c #E9EBEE",
    "m+	c #FBFBFB",
    "n+	c #F9F9FA",
    "o+	c #E7E9EC",
    "p+	c #F7F8F8",
    "q+	c #E0E3E7",
    "r+	c #98AFCF",
    "s+	c #F1EFDE",
    "t+	c #EBEBEC",
    "u+	c #B1D5ED",
    "v+	c #DEE8F0",
    "w+	c #1B60A2",
    "x+	c #E7EBF0",
    "y+	c #F0F3F7",
    "z+	c #E8E8E8",
    "A+	c #E3EEF7",
    "B+	c #EFB521",
    "C+	c #DDD9CA",
    "D+	c #E4AF2A",
    "E+	c #EBECE7",
    "F+	c #E7E7E9",
    "G+	c #EAB930",
    "c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+",
    "c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+",
    "c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+c+",
    "h+e+h+e+e+h+e+h+e+e+h+e+e+h+e+h+",
    "a+0+a+0+0+a+0+a+0+0+a+0+0+a+0+a+",
    "f+7 f+f+7 f+f+f+f+7 f+7 7 f+4.f+",
    "a c . C.c a b A+< 1 r.a a 4.$ a ",
    "~.+.+.@ 1+' ~+|.h.F F O L F ! Q.",
    "; +.D T w ( ' 1 . 1 k :+h k F . ",
    "g w _.@ w z 8 + x Z.~ E 8 ! F z ",
    "' w ( ~ +.v ~ 0.M.k+} E.E 1 1 |.",
    "8 f _+_+f 9 5 1 1 1 B 2 M.! M.' ",
    "| [.o+l+D.| #.D.t 4 R : Q >.>.m ",
    "g | { q+g   U & %.& | U g i N.{ ",
    "& ^ & ^ ^ & ^ & ^ ^ & ^ ^ & ^ & ",
    "{ ) { { ) { { { { ) { ) ) { ) { "};

    return QIcon(QPixmap(icon));
}


bool QNapisy24Engine::isConfigurable()
{
    return true;
}

void QNapisy24Engine::configure(QWidget * parent)
{
    frmNapisy24Config config(parent);
    config.exec();
}

QString QNapisy24Engine::checksum(QString filename)
{
    if(filename.isEmpty())
        filename = movie;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return QString("");

    fileSize = file.size();
    quint64 hash = fileSize;
    quint64 tmp, i;

    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;
    file.seek(qMax(0, (int)((qint64)fileSize - 65536)));
    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;

    return (checkSum = QString("%1").arg(hash, 16, 16, QChar('0')));
}

bool QNapisy24Engine::lookForSubtitles(QString lang)
{
    if(lang != "pl") return false;

    const QUrl url = QUrl("http://napisy24.pl/run/CheckSubAgent.php");

    auto credentials = getCredentials();

    QUrlQuery params(url);
    params.addQueryItem("postAction", "CheckSub");
    params.addQueryItem("ua", credentials.first);
    params.addQueryItem("ap", credentials.second);
    params.addQueryItem("fh", checkSum);
    params.addQueryItem("fs", QString::number(fileSize));
    params.addQueryItem("fn", QFileInfo(movie).fileName());
    // API N24 ignoruje ten parametr; jeśli nie ma napisów w żądanym języku, to i tak
    // zwraca napisy w jęz. polskim
    // params.addQueryItem("nl", lang);
    QByteArray data = params.query().toUtf8();

    SyncHTTP http;
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = http.syncPost(req, data);
    if(reply->error() != QNetworkReply::NoError)
        return false;

    QByteArray respData = reply->readAll();
    if (!respData.startsWith("OK-2"))
        return false;

    QString tmpPackedFile = generateTmpPath();

    QFile file(tmpPackedFile);
    if(file.exists()) file.remove();
    if(!file.open(QIODevice::WriteOnly))
        return false;

    int r = file.write(respData.mid(respData.indexOf("||")+2));
    file.close();
    if (!r)
        return false;

    subtitlesList << QNapiSubtitleInfo( lang,
                                        engineName(),
                                        tmpPackedFile,
                                        QFileInfo(movie).completeBaseName(),
                                        "",
                                        "txt",
                                        SUBTITLE_UNKNOWN);

    return true;
}

QList<QNapiSubtitleInfo> QNapisy24Engine::listSubtitles()
{
    return subtitlesList;
}

bool QNapisy24Engine::download(QUuid id)
{
    Maybe<QNapiSubtitleInfo> ms = resolveById(id);

    return ms && QFile::exists(ms.value().sourceLocation);
}

bool QNapisy24Engine::unpack(QUuid id)
{
    Maybe<QNapiSubtitleInfo> ms = resolveById(id);
    if(!ms) return false;

    if(!QFile::exists(movie)) return false;

    QProcess p7zip;

    p7zip.start(p7zipPath, QStringList() << "l" << "-slt" << ms.value().sourceLocation);
    if (!p7zip.waitForFinished(5000))
        return false;

    QString resp = QByteArray(p7zip.readAll());

#ifdef Q_OS_WIN
    QRegExp r("\r\nPath = ([^\r\n]*)\r\n");
#else
    QRegExp r("\nPath = ([^\n]*)\n");
#endif
    r.setPatternSyntax(QRegExp::RegExp2);

    QStringList pathMatches;
    int offset = 0;

    while((offset = r.indexIn(resp, offset)) != -1)
    {
        QString filePath = r.cap(1);
        pathMatches << filePath;
        offset += r.matchedLength();
    }

    QString subFileName = "";
    QStringList subExts = GlobalConfig().subtitleExtensions();
    foreach (QString archiveFileName, pathMatches) {
        if(subExts.contains(QFileInfo(archiveFileName).suffix(), Qt::CaseInsensitive)) {
            subFileName = archiveFileName;
            break;
        }
    }

    if(subFileName.isEmpty())
        return false;

    subtitlesTmp = tmpPath + "/" + subFileName;
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);

    QStringList args;
    args << "e" << "-y" << ("-o" + tmpPath) << ms.value().sourceLocation << subFileName;

    p7zip.start(p7zipPath, args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;
    return QFile::exists(subtitlesTmp);
}

void QNapisy24Engine::cleanup()
{
    clearSubtitlesList();
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);
}

QPair<QString, QString> QNapisy24Engine::getCredentials() const
{
    QString configLogin = GlobalConfig().nick("Napisy24");
    QString configPass = GlobalConfig().pass("Napisy24");

    if(!configLogin.isEmpty() && !configPass.isEmpty()) {
        return qMakePair(configLogin, configPass);
    } else {
        return qMakePair(QString("tantalosus"), QString("susolatnat"));
    }
}

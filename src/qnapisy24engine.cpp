#include "qnapisy24engine.h"
#include <QNetworkRequest>
#include <QNetworkReply>

QNapisy24Engine::QNapisy24Engine(const QString & movieFile, const QString & subtitlesFile)
    : QNapiAbstractEngine(movieFile, subtitlesFile)
{
    p7zipPath = GlobalConfig().p7zipPath();
    lang = GlobalConfig().language();
    noBackup = GlobalConfig().noBackup();
    tmpPackedFile =  QString("%1/%2").arg(tmpPath).arg(generateTmpFileName());
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
    return "Modul pobierania napisów z bazy <b>www.napisy24.pl</b><br />";
}

QIcon QNapisy24Engine::engineIcon()
{
    static const char *icon[] = {
    "16 16 256 2 ",
    "   c #FFFFFF",
    ".  c #FFFFFF",
    "X  c #FFFFFF",
    "o  c #FFFFFF",
    "O  c #FFFFFF",
    "+  c #FFFFFF",
    "@  c #FFFFFF",
    "#  c #FFFFFF",
    "$  c #FFFFFF",
    "%  c #FFFFFF",
    "&  c #FFFFFF",
    "*  c #FFFFFF",
    "=  c #FFFFFF",
    "-  c #FFFFFF",
    ";  c #FFFFFF",
    ":  c #FFFFFF",
    ">  c #FFFFFF",
    ",  c #FFFFFF",
    "<  c #FFFFFF",
    "1  c #FFFFFF",
    "2  c #FFFFFF",
    "3  c #FFFFFF",
    "4  c #FFFFFF",
    "5  c #FFFFFF",
    "6  c #FFFFFF",
    "7  c #FFFFFF",
    "8  c #FFFFFF",
    "9  c #FFFFFF",
    "0  c #FFFFFF",
    "q  c #FFFFFF",
    "w  c #FFFFFF",
    "e  c #FFFFFF",
    "r  c #FFFFFF",
    "t  c #FDFDFD",
    "y  c #FBFBFB",
    "u  c #FBFBFB",
    "i  c #FBFBFB",
    "p  c #FBFBFB",
    "a  c #FBFBFB",
    "s  c #FBFBFB",
    "d  c #FAFAFA",
    "f  c #F8F8F8",
    "g  c #F7F7F7",
    "h  c #F7F6F6",
    "j  c #F5F6F5",
    "k  c #F2F4F3",
    "l  c #F6F6F6",
    "z  c #FFFFFF",
    "x  c #FFFFFF",
    "c  c #FCFCFC",
    "v  c #FAFBFB",
    "b  c #FBFBFB",
    "n  c #FBFBFB",
    "m  c #FAFBFB",
    "M  c #FBFBFB",
    "N  c #FAFAFA",
    "B  c #F6F8F8",
    "V  c #F4F6F7",
    "C  c #F6F6F5",
    "Z  c #F4F5F5",
    "A  c #F2F2F1",
    "S  c #EFF2F1",
    "D  c #F3F4F4",
    "F  c #FFFFFF",
    "G  c #FFFFFF",
    "H  c #FCFDFC",
    "J  c #FAFCFB",
    "K  c #FAFCFB",
    "L  c #FAFCFC",
    "P  c #FAFCFA",
    "I  c #FAFCF9",
    "U  c #F9FBFA",
    "Y  c #FFFFFD",
    "T  c #FFFFFC",
    "R  c #F4F7F6",
    "E  c #F7F9F6",
    "W  c #FBFBF8",
    "Q  c #EFF2F0",
    "!  c #F3F4F4",
    "~  c #FFFFFF",
    "^  c #FFFFFF",
    "/  c #FBFCFC",
    "(  c #F9FAF8",
    ")  c #F9F9F7",
    "_  c #F7F9F7",
    "`  c #FAFDFF",
    "'  c #F9FCFF",
    "]  c #FDFDFB",
    "[  c #C0D6E8",
    "{  c #BACFE5",
    "}  c #FFFFF8",
    "|  c #D3E1EC",
    " . c #799DC7",
    ".. c #F2F2F0",
    "X. c #F2F3F3",
    "o. c #FFFFFF",
    "O. c #FFFFFF",
    "+. c #FBFBFA",
    "@. c #F8F9FB",
    "#. c #F5F2DD",
    "$. c #F3CF73",
    "%. c #F3E4B2",
    "&. c #FFECBB",
    "*. c #BED3E6",
    "=. c #3D76B5",
    "-. c #1D5DAF",
    ";. c #C6D7E3",
    ":. c #4E84C0",
    ">. c #3F74B6",
    ",. c #F8F9F3",
    "<. c #F0F0F1",
    "1. c #FFFFFF",
    "2. c #FFFFFF",
    "3. c #FBFBFA",
    "4. c #F6F8FB",
    "5. c #F7F1DE",
    "6. c #F6CA48",
    "7. c #F2CF62",
    "8. c #FAD35D",
    "9. c #E7F0F8",
    "0. c #90B5D5",
    "q. c #5A8EC2",
    "w. c #6C9DCA",
    "e. c #3C7BBA",
    "r. c #6E9BCC",
    "t. c #FBF6F5",
    "y. c #EFF0F1",
    "u. c #FFFFFF",
    "i. c #FFFFFF",
    "p. c #FAF9F8",
    "a. c #F9FBFF",
    "s. c #FAF0D1",
    "d. c #F6D97D",
    "f. c #F7EBBE",
    "g. c #FFDE77",
    "h. c #B1CBE5",
    "j. c #2567B5",
    "k. c #6B9ECD",
    "l. c #3371BB",
    "z. c #2970B9",
    "x. c #3A77B8",
    "c. c #DDE5EA",
    "v. c #F5F4F3",
    "b. c #FEFFFF",
    "n. c #FFFFFF",
    "m. c #FAFAFA",
    "M. c #F7F7F7",
    "N. c #F7F6F4",
    "B. c #FAFAF9",
    "V. c #FAFBFA",
    "C. c #FEFCEF",
    "Z. c #CDD9E8",
    "A. c #6795C3",
    "S. c #6D97C5",
    "D. c #E4EBEE",
    "F. c #3375B9",
    "G. c #769FCF",
    "H. c #FCF9F3",
    "J. c #ECEDEF",
    "K. c #FFFFFF",
    "L. c #FFFFFF",
    "P. c #FAFAFA",
    "I. c #F6F7F6",
    "U. c #F6F7F7",
    "Y. c #F5F5F8",
    "T. c #F4F4F5",
    "R. c #F2F4F5",
    "E. c #FDFCF9",
    "W. c #FFFFFB",
    "Q. c #FFFFFD",
    "!. c #FFFEF8",
    "~. c #C2D4E6",
    "^. c #C7D6E6",
    "/. c #EDEDED",
    "(. c #EEEEEF",
    "). c #FFFFFF",
    "_. c #FFFFFF",
    "`. c #F9FAF9",
    "'. c #F4F6F5",
    "]. c #F5F7F6",
    "[. c #F5F7F6",
    "{. c #F5F7F6",
    "}. c #F5F7F6",
    "|. c #F3F6F5",
    " X c #F2F5F5",
    ".X c #F2F6F5",
    "XX c #F4F6F6",
    "oX c #FAF8F7",
    "OX c #F3F3F2",
    "+X c #E8E9EC",
    "@X c #EFEFF0",
    "#X c #FFFFFF",
    "$X c #FFFFFF",
    "%X c #F8F9F9",
    "&X c #F4F6F5",
    "*X c #F5F7F6",
    "=X c #F4F6F5",
    "-X c #F4F6F5",
    ";X c #F4F6F5",
    ":X c #F4F6F5",
    ">X c #F5F7F6",
    ",X c #F2F4F4",
    "<X c #EFF1F1",
    "1X c #EBEDF0",
    "2X c #ECEDEF",
    "3X c #ECEDEE",
    "4X c #F1F1F2",
    "5X c #FFFFFF",
    "6X c #FFFFFF",
    "7X c #F9FAFA",
    "8X c #F5F8F7",
    "9X c #F6F8F7",
    "0X c #F6F8F7",
    "qX c #F5F7F6",
    "wX c #F6F8F7",
    "eX c #F6F8F7",
    "rX c #F5F7F6",
    "tX c #F1F2F4",
    "yX c #F1F2F4",
    "uX c #F2F3F5",
    "iX c #F3F5F6",
    "pX c #F2F4F3",
    "aX c #F5F6F6",
    "sX c #FFFFFF",
    "dX c #FFFFFF",
    "fX c #FFFFFF",
    "gX c #FEFFFF",
    "hX c #FEFFFF",
    "jX c #FEFFFF",
    "kX c #FEFFFF",
    "lX c #FEFFFF",
    "zX c #FEFFFF",
    "xX c #FEFFFE",
    "cX c #FEFFFF",
    "vX c #FFFFFF",
    "bX c #FFFFFF",
    "nX c #FFFFFF",
    "mX c #FFFFFF",
    "MX c #FFFFFF",
    "NX c #FFFFFF",
    "BX c #FFFFFF",
    "VX c #FFFFFF",
    "CX c #FFFFFF",
    "ZX c #FFFFFF",
    "AX c #FFFFFF",
    "SX c #FFFFFF",
    "DX c #FFFFFF",
    "FX c #FFFFFF",
    "GX c #FFFFFF",
    "HX c #FFFFFF",
    "JX c #FFFFFF",
    "KX c #FFFFFF",
    "LX c #FFFFFF",
    "PX c #FFFFFF",
    "IX c #FFFFFF",
    "UX c #FFFFFF",
    /* pixels */
    "  . X o O + @ # $ % & * = - ; : ",
    "> , < 1 2 3 4 5 6 7 8 9 0 q w e ",
    "r t y u i p a s d f g h j k l z ",
    "x c v b n m M N B V C Z A S D F ",
    "G H J K L P I U Y T R E W Q ! ~ ",
    "^ / ( ) _ ` ' ] [ { } |  ...X.o.",
    "O.+.@.#.$.%.&.*.=.-.;.:.>.,.<.1.",
    "2.3.4.5.6.7.8.9.0.q.w.e.r.t.y.u.",
    "i.p.a.s.d.f.g.h.j.k.l.z.x.c.v.b.",
    "n.m.M.N.B.V.C.Z.A.S.D.F.G.H.J.K.",
    "L.P.I.U.Y.T.R.E.W.Q.!.~.^./.(.).",
    "_.`.'.].[.{.}.|. X.XXXoXOX+X@X#X",
    "$X%X&X*X=X-X;X:X>X,X<X1X2X3X4X5X",
    "6X7X8X9X0XqXwXeXrXtXyXuXiXpXaXsX",
    "dXfXgXhXjXkXlXzXxXcXvXbXnXmXMXNX",
    "BXVXCXZXAXSXDXFXGXHXJXKXLXPXIXUX"
    };

    return QIcon(QPixmap(icon));
}


bool QNapisy24Engine::isConfigurable()
{
    return false;
}

void QNapisy24Engine::configure(QWidget * parent)
{
    Q_UNUSED(parent);
}

QString QNapisy24Engine::checksum(QString filename)
{
    if(filename.isEmpty())
        filename = movie;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return QString("");

    fileSize = file.size();
    QFileInfo fileInfo(file.fileName());
    fileName = fileInfo.fileName();
    quint64 hash = fileSize;
    quint64 tmp, i;

    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;
    file.seek(qMax(0, (int)((qint64)fileSize - 65536)));
    for(tmp = 0, i = 0; i < 65536/sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp) ;

    return (checkSum = QString("%1").arg(hash, 16, 16, QChar('0')));
}

bool QNapisy24Engine::lookForSubtitles(QString lang)
{
    subtitlesList.clear();
    const QUrl url = QUrl("http://napisy24.pl/run/CheckSubAgent.php");

    QUrlQuery params(url);
    params.addQueryItem("postAction", "CheckSub");
    params.addQueryItem("ua", "tantalosus");
    params.addQueryItem("ap", "susolatnat");
    params.addQueryItem("fh", checkSum);
    params.addQueryItem("fs", QString::number(fileSize));
    params.addQueryItem("fn", fileName);
    params.addQueryItem("nl", lang); // TODO: Add napiprojekt checksum
    QByteArray data = params.query().toUtf8();

    SyncHTTP http;
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = http.syncPost(req, data);
    if(reply->error() != QNetworkReply::NoError)
        return false;

    QByteArray respData = reply->readAll();
    if (not respData.startsWith("OK-2"))
        return false;

    QFile file(tmpPackedFile);
    if(file.exists()) file.remove();
    if(!file.open(QIODevice::WriteOnly))
        return false;

    int r = file.write(respData.mid(respData.indexOf("||")+2));
    file.close();
    if (!r)
        return false;

    subtitlesList << QNapiSubtitleInfo(	lang,
                                        engineName(),
                                        tmpPackedFile,
                                        QFileInfo(movie).completeBaseName(),
                                        "",
                                        "txt",
                                        SUBTITLE_UNKNOWN);

    return (subtitlesList.size() > 0);
}

QList<QNapiSubtitleInfo> QNapisy24Engine::listSubtitles()
{
    return subtitlesList;
}

bool QNapisy24Engine::download(int idx)
{
    Q_UNUSED(idx);
    return (subtitlesList.size() > 0);
}

bool QNapisy24Engine::unpack()
{
    if(!QFile::exists(tmpPackedFile)) return false;
    if(!QFile::exists(movie)) return false;

    QProcess p7zip;

    p7zip.start(p7zipPath, QStringList() << "l" << tmpPackedFile);
    if (!p7zip.waitForFinished(5000))
        return false;

    QString resp = QByteArray(p7zip.readAll());
    QRegExp r("\n\\d{4}-\\d{2}-\\d{2} +\\d{1,2}:\\d{2}:\\d{1,2} +\\.+ +\\d+ +\\d+ +([^\n]+)\n");
    r.setPatternSyntax(QRegExp::RegExp2);
    if (r.indexIn(resp) == -1)
        return false;

    // subtitlesTmp = tmpPath + "/" + QFileInfo(fileName).completeBaseName() + ".txt";
    subtitlesTmp = tmpPath + "/" + r.cap(1);
    if(QFile::exists(subtitlesTmp))
        QFile::remove(subtitlesTmp);

    QStringList args;
    args << "e" << "-y" << ("-o" + tmpPath) << tmpPackedFile;

    // QProcess p7zip;
    p7zip.start(p7zipPath, args);

    // Rozpakowujemy napisy max w ciagu 5 sekund
    if(!p7zip.waitForFinished(5000)) return false;
    return QFile::exists(subtitlesTmp);
}

void QNapisy24Engine::cleanup()
{

}

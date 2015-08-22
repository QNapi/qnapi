#include "microdvd.h"
#include <QDebug>

bool MicroDVDSubtitleFormat::detect(const QStringList &lines)
{
    foreach(QString line, lines)
    {
        if(!line.trimmed().isEmpty())
        {
            QRegExp r("^\\{(\\d+)\\}\\{(\\d+)\\}(.*)");
            r.setPatternSyntax(QRegExp::RegExp2);
            return r.exactMatch(line);
        }
    }

    return false;
}

SubFile MicroDVDSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;

    foreach(QString line, lines)
    {
        if(!line.trimmed().isEmpty())
        {
            QRegExp r("^\\{(\\d+)\\}\\{(\\d+)\\}(.*)");
            r.setPatternSyntax(QRegExp::RegExp2);
            if(r.exactMatch(line))
            {
                SubEntry se;
                se.frameStart = r.cap(1).toLong();
                se.frameStop = r.cap(2).toLong();
                QString tokenStream = r.cap(3);

                qDebug() << se.frameStart << se.frameStop << tokenStream;

                se.tokens = decodeTokenStream(tokenStream);

                sf.entries.push_back(se);
            }
        }
    }

    return sf;
}

QStringList MicroDVDSubtitleFormat::encode(const SubFile & subFile)
{
    QStringList lines;
    foreach(const SubEntry & entry, subFile.entries)
    {
        lines.append(encodeEntry(entry));
    }
    return lines;
}

QString MicroDVDSubtitleFormat::encodeEntry(const SubEntry & entry)
{
    QString tokensBuff;
    foreach(const SubToken & token, entry.tokens)
    {
        tokensBuff += encodeToken(token);
    }
    return QString("{%1}{%2}%3")
            .arg(entry.frameStart)
            .arg(entry.frameStop)
            .arg(tokensBuff);
}

QString MicroDVDSubtitleFormat::encodeToken(const SubToken & entry)
{
    switch(entry.type)
    {
    case STT_WS: return " ";
    case STT_WORD: return entry.payload;
    case STT_NEWLINE: return "|";
    case STT_BOLD: return "{y:b}";
    case STT_ITALIC: return "{y:i}";
    case STT_UNDERLINE: return "{y:u}";
    case STT_FONTCOLOR: return QString("{c:%1}").arg(entry.payload);
    default: return "";
    }
}

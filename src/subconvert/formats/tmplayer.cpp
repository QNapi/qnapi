#include "tmplayer.h"


bool TMPlayerSubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile TMPlayerSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList TMPlayerSubtitleFormat::encode(const SubFile & subFile)
{
    QStringList lines;
    foreach(const SubEntry & entry, subFile.entries)
    {
        lines.append(encodeEntry(entry));
    }
    return lines;
}

QString TMPlayerSubtitleFormat::encodeEntry(const SubEntry & entry)
{
    QString tokensBuff;
    foreach(const SubToken & token, entry.tokens)
    {
        tokensBuff += encodeToken(token);
    }
    long ss = entry.frameStart / 1000;
    int h = ss / 3600;
    int m = (ss - h * 3600) / 60;
    int s = ss - h * 3600 - m * 60;
    return QString("%1:%2:%3: %4")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
            .arg(tokensBuff);
}

QString TMPlayerSubtitleFormat::encodeToken(const SubToken & entry)
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

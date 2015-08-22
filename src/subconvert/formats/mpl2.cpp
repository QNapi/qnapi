#include "mpl2.h"

bool MPL2SubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile MPL2SubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList MPL2SubtitleFormat::encode(const SubFile & subFile)
{
    QStringList lines;
    foreach(const SubEntry & entry, subFile.entries)
    {
        lines.append(encodeEntry(entry));
    }
    return lines;
}

QString MPL2SubtitleFormat::encodeEntry(const SubEntry & entry)
{
    QString tokensBuff;
    foreach(const SubToken & token, entry.tokens)
    {
        tokensBuff += encodeToken(token);
    }
    return QString("[%1][%2]%3")
            .arg(entry.frameStart / 100)
            .arg(entry.frameStop / 100)
            .arg(tokensBuff);
}

QString MPL2SubtitleFormat::encodeToken(const SubToken & entry)
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

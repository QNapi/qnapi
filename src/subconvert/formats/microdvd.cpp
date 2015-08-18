#include "microdvd.h"

bool MicroDVDSubtitleFormat::detect(const QStringList &lines)
{
    // find first trimmed non-empty line
    // result = does it match to regexp {\d}{\d}(.*)
    //
    return false;
}

SubFile MicroDVDSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    sf.isTimeBased = false;
    // match each trimmed non-empty line to regexp {\d}{\d}(.*)
    // extract SubEntry, add to sf.entries
    //sf.entries
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
    return QString("{%1}{%2}%3\r\n")
            .arg(entry.frameStart, entry.frameStop)
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

#include "subrip.h"


bool SubRipSubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile SubRipSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList SubRipSubtitleFormat::encode(const SubFile & subFile)
{
    QStringList lines;
    int i = 1;
    foreach(const SubEntry & entry, subFile.entries)
    {
        lines << encodeEntry(entry, i++);
    }
    return lines;
}

QStringList SubRipSubtitleFormat::encodeEntry(const SubEntry & entry, int i)
{
    QString tokensBuff;
    foreach(const SubToken & token, entry.tokens)
    {
        tokensBuff += encodeToken(token);
    }

    int h1 = entry.frameStart / 3600000;
    int m1 = (entry.frameStart - h1 * 3600000) / 60000;
    int s1 = (entry.frameStart - h1 * 3600000 - m1 * 60000) / 1000;
    int ms1 = entry.frameStart - h1 * 3600000 - m1 * 60000 - s1 * 1000;

    int h2 = entry.frameStop / 3600000;
    int m2 = (entry.frameStop - h2 * 3600000) / 60000;
    int s2 = (entry.frameStop - h2 * 3600000 - m2 * 60000) / 1000;
    int ms2 = entry.frameStop - h2 * 3600000 - m2 * 60000 - s2 * 1000;

    QStringList out;
    out << QString::number(i)
        << QString("%1:%2:%3,%4 --> %5:%6:%7,%8")
           .arg(h1, 2, 10, QChar('0'))
           .arg(m1, 2, 10, QChar('0'))
           .arg(s1, 2, 10, QChar('0'))
           .arg(ms1, 3, 10, QChar('0'))
           .arg(h2, 2, 10, QChar('0'))
           .arg(m2, 2, 10, QChar('0'))
           .arg(s2, 2, 10, QChar('0'))
           .arg(ms2, 3, 10, QChar('0'))
        << tokensBuff.split("\r\n");
    return out;
}

QString SubRipSubtitleFormat::encodeToken(const SubToken & entry)
{
    switch(entry.type)
    {
    case STT_WS: return " ";
    case STT_WORD: return entry.payload;
    case STT_NEWLINE: return "\r\n";
    case STT_BOLD: return "<b>";
    case STT_BOLD_END: return "</b>";
    case STT_ITALIC: return "<i>";
    case STT_ITALIC_END: return "</i>";
    case STT_UNDERLINE: return "<u>";
    case STT_UNDERLINE_END: return "</u>";
    case STT_FONTCOLOR: return QString("<font color=\"%1\">").arg(entry.payload);
    case STT_FONTCOLOR_END: return "</font>";
    default: return "";
    }
}

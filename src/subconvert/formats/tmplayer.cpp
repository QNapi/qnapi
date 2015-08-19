#include "tmplayer.h"
#include <QMap>

bool TMPlayerSubtitleFormat::detect(const QStringList &lines)
{
    foreach(QString line, lines)
    {
        if(!line.trimmed().isEmpty())
        {
            QRegExp r("^(\\d){1,2}:(\\d){1,2}:(\\d){1,2}:(.*)");
            r.setPatternSyntax(QRegExp::RegExp2);
            return r.indexIn(line) == 0;
        }
    }

    return false;
}

SubFile TMPlayerSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;

    foreach(QString line, lines)
    {
        if(!line.trimmed().isEmpty())
        {
            QRegExp r("^(\\d){1,2}:(\\d){1,2}:(\\d){1,2}:(.*)");
            r.setPatternSyntax(QRegExp::RegExp2);
            if(r.indexIn(line))
            {
                int h = r.cap(1).toInt();
                int m = r.cap(2).toInt();
                int s = r.cap(3).toInt();
                QString tokenStream = r.cap(4);

                SubEntry se;
                se.frameStart = 1000L * (3600L * h + 60L * m + s);
                se.tokens = decodeTokenStream(tokenStream);

                sf.entries.push_back(se);
            }
        }
    }

    for(int i = 0; i < sf.entries.size() - 1; ++i)
    {
        long plus4s = sf.entries[i].frameStart + 4000L;
        sf.entries[i].frameStop = std::min(sf.entries[i + 1].frameStart - 1, plus4s);
    }

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

QVector<SubToken> TMPlayerSubtitleFormat::decodeTokenStream(QString tokenStream)
{
    QVector<SubToken> tokens;
    SubToken tok;

    QMap<QString, SubTokenType> tokenTypes = {
        {"{y:b}", STT_BOLD}, {"{b}", STT_BOLD}, {"<b>", STT_BOLD},
        {"{y:i}", STT_ITALIC}, {"{i}", STT_ITALIC}, {"<i>", STT_ITALIC},
        {"{y:u}", STT_UNDERLINE}, {"{u}", STT_UNDERLINE}, {"<u>", STT_UNDERLINE},
        {"{/y:b}", STT_BOLD_END}, {"{/b}", STT_BOLD_END}, {"</b>", STT_BOLD_END},
        {"{/y:i}", STT_ITALIC_END}, {"{/i}", STT_ITALIC_END}, {"</i>", STT_ITALIC_END},
        {"{/y:u}", STT_UNDERLINE_END}, {"{/u}", STT_UNDERLINE_END}, {"</u>", STT_UNDERLINE_END},
        {"{/c}", STT_FONTCOLOR_END}, {"</font>", STT_FONTCOLOR_END},
        {"/", STT_ITALIC}, {"|", STT_NEWLINE}
    };

    QString wordBuff;

    while(!tokenStream.isEmpty())
    {
        tok.payload.clear();

        bool matched = false;

        foreach(QString token, tokenTypes.keys())
        {
            if(tokenStream.startsWith(token, Qt::CaseInsensitive))
            {
                tok.type = tokenTypes[token];
                tokenStream.remove(0, token.size());
                matched = true;
                break;
            }
        }

        if(!matched)
        {
            QRegExp colorR1("^\\{c:[$#](.*)\\}");
            colorR1.setPatternSyntax(QRegExp::RegExp2);
            colorR1.setCaseSensitivity(Qt::CaseInsensitive);

            QRegExp colorR2("^<font color=\"[$#](.*)\">");
            colorR1.setPatternSyntax(QRegExp::RegExp2);
            colorR1.setCaseSensitivity(Qt::CaseInsensitive);


            if(colorR1.indexIn(tokenStream) == 0)
            {
                tok.type = STT_FONTCOLOR;
                tok.payload = colorR1.cap(2);
                tokenStream.remove(0, colorR1.cap(0).size());
                matched = true;
            } else if(colorR2.indexIn(tokenStream) == 0)
            {
                tok.type = STT_FONTCOLOR;
                tok.payload = colorR2.cap(2);
                tokenStream.remove(0, colorR2.cap(0).size());
                matched = true;
            } else if(tokenStream[0].isSpace()) {
                while(tokenStream.isEmpty() || !tokenStream[0].isSpace())
                    tokenStream.remove(0, 1);
                tok.type = STT_WS;
                matched = true;
            } else {
                wordBuff += tokenStream[0];
                tokenStream.remove(0, 1);
            }
        }

        if(matched)
        {
            if(!wordBuff.isEmpty())
            {
                SubToken wordTok;
                wordTok.type = STT_WORD;
                wordTok.payload = wordBuff;
                tokens.push_back(wordTok);
                wordBuff.clear();
            }
            tokens.push_back(tok);
        }
    }

    if(!wordBuff.isEmpty())
    {
        SubToken wordTok;
        wordTok.type = STT_WORD;
        wordTok.payload = wordBuff;
        tokens.push_back(wordTok);
    }

    return tokens;
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
    case STT_FONTCOLOR: return QString("{c:$%1}").arg(entry.payload);
    default: return "";
    }
}

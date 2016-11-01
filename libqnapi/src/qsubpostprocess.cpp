#include "qsubpostprocess.h"
#include "qnapiconfig.h"
#include "subconvert/subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"

#include <QTextCodec>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

void QSubPostProcess::perform()
{
    if(GlobalConfig().ppRemoveLines())
    {
        ppRemoveLinesContainingWords(GlobalConfig().ppRemoveWords());
    }

    switch (GlobalConfig().ppEncodingMethod()) {
        case CEM_REPLACE_DIACRITICS:
            ppReplaceDiacriticsWithASCII();
        break;
        case CEM_CHANGE:
            if (!GlobalConfig().ppAutoDetectEncoding() || !ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingTo()))
            {
                ppChangeSubtitlesEncoding(GlobalConfig().ppEncodingFrom(),
                                          GlobalConfig().ppEncodingTo());
            }
        break;
        case CEM_ORIGINAL:
            // Nie ruszaj pobranych napisów!
        break;
    }

    if(!GlobalConfig().ppSubFormat().isEmpty())
    {
        SubtitleConverter sc;
        QString targetFormat = GlobalConfig().ppSubFormat();
        sc.convertSubtitles(subtitleFilePath, targetFormat, subtitleFilePath, movieFilePath);
    }
}


bool QSubPostProcess::ppReplaceDiacriticsWithASCII()
{
    if(!QFileInfo(subtitleFilePath).exists())
        return false;

    QString from = encodingUtils.detectFileEncoding(subtitleFilePath);

    if(from.isEmpty())
        return false;

    QFile f(subtitleFilePath);
    if(!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray fileContent = f.readAll();

    QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
    f.close();

    fileContent.clear();
    fileContent.append(encodingUtils.replaceDiacriticsWithASCII(contentStr));

    if(!f.open(QIODevice::WriteOnly))
        return false;

    f.write(fileContent);
    f.close();

    return true;
}

bool QSubPostProcess::ppChangeSubtitlesEncoding(const QString & from, const QString & to)
{
    QFile f(subtitleFilePath);
    if(!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray fileContent = f.readAll();

    QString contentStr = QTextCodec::codecForName(qPrintable(from))->toUnicode(fileContent);
    f.close();

    if(to.compare("UTF-8", Qt::CaseInsensitive) != 0)
    {
        fileContent = QTextCodec::codecForName(qPrintable(to))
                        ->fromUnicode(contentStr.constData(), contentStr.size());
    }
    else
    {
        fileContent.clear();
        fileContent.append(contentStr);
    }

    if(!f.open(QIODevice::WriteOnly))
        return false;

    f.write(fileContent);
    f.close();

    return true;
}

bool QSubPostProcess::ppChangeSubtitlesEncoding(const QString & to)
{
    if(!QFileInfo(subtitleFilePath).exists())
        return false;

    QString from = encodingUtils.detectFileEncoding(subtitleFilePath);

    if(from.isEmpty())
        return false;

    return ppChangeSubtitlesEncoding(from, to);
}

bool QSubPostProcess::ppRemoveLinesContainingWords(QStringList wordList)
{
    if(!QFileInfo(subtitleFilePath).exists())
        return false;

    wordList = wordList.filter(QRegExp("^(.+)$"));

    QString fromCodec = encodingUtils.detectFileEncoding(subtitleFilePath);

    if(fromCodec.isEmpty())
        fromCodec = GlobalConfig().ppEncodingFrom();

    QFile f(subtitleFilePath);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QList<QByteArray> lines = f.readAll().split('\n');
    QList<QByteArray> out;

    foreach(QByteArray line, lines)
    {
        int i;
        while((i = line.indexOf('\r')) >= 0)
            line.remove(i, 1);

        QTextStream ts(line);
        ts.setCodec(qPrintable(fromCodec));
        QString encLine = ts.readAll();

        if(encLine.isEmpty())
        {
            out << line;
            continue;
        }

        bool found = false;
        foreach(QString word, wordList)
        {
            if(encLine.contains(word, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }
        if(found) continue;

        out << line;
    }

    f.close();

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    foreach(QByteArray line, out)
    {
        f.write(line);
        f.write("\n", 1);
    }
    f.close();

    return true;
}

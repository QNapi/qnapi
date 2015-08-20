#include "subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"
#include "qnapiconfig.h"
#include "ffprobemovieinfoparser.h"
#include "movieinfo.h"
#include <cmath>
#include <QTextStream>
#include <QDebug>

QString SubtitleConverter::detectFormat(const QString &subtitleFile)
{
    const QStringList & lines = readFile(subtitleFile, 15);
    return detectFormat(lines);
}

QString SubtitleConverter::detectFormat(const QStringList & subtitleLines)
{
    foreach(QString format, GlobalFormatsRegistry().enumerateFormats())
    {
        if(GlobalFormatsRegistry().select(format)->detect(subtitleLines))
        {
            return format;
        }
    }
    return "";
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetFileName,
                                         double movieFPS,
                                         double fpsRatio)
{

    return convertSubtitles(subtitleFile, targetFormatName, targetFileName, [=]() -> double { return movieFPS; }, fpsRatio);
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetExt,
                                         QString movieFile)
{
    if(targetExt.isEmpty())
        targetExt = GlobalFormatsRegistry().select(targetFormatName)->defaultExtension();

    QString targetFileName = QFileInfo(subtitleFile).completeBaseName() + "." + targetExt;

    return convertSubtitles(subtitleFile, targetFormatName, targetFileName, [&]() -> double {
        FFProbeMovieInfoParser mip(GlobalConfig().ffProbePath());
        MovieInfo mfm = mip.parseFile(movieFile);
        if(mfm.isFilled)
            return mfm.frameRate;
        else
            return 0.0;
    });
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile, QString targetFormatName, QString targetFileName, std::function<double ()> determineFPS, double fpsRatio)
{
    QStringList subtitleLines = readFile(subtitleFile);

    QString detectedFormat = detectFormat(subtitleLines);
    if(detectedFormat.isEmpty())
        return false;

    SubtitleFormat * inputFormat = GlobalFormatsRegistry().select(detectedFormat);
    SubtitleFormat * targetFormat = GlobalFormatsRegistry().select(targetFormatName);

    if(inputFormat == targetFormat)
        return true;

    SubFile sf = inputFormat->decode(subtitleLines);

    qDebug() << "ENTRIES:" << sf.entries.size();

    if(inputFormat->isTimeBased() != targetFormat->isTimeBased())
    {
        double frameRate = determineFPS();
        if(frameRate == 0.0)
            return false;

        if(targetFormat->isTimeBased())
        { // but input not
            // we have to convert frames to times
            foreach(SubEntry se, sf.entries)
            {
                se.frameStart = (long)floor(1000.0 * se.frameStart / frameRate);
                se.frameStop = (long)floor(1000.0 * se.frameStop / frameRate);
            }
        }
        else
        {
            // we have to convert times to frames
            foreach(SubEntry se, sf.entries)
            {
                se.frameStart = (long)floor(frameRate * se.frameStart / 1000.0);
                se.frameStop = (long)floor(frameRate * se.frameStop / 1000.0);
            }
        }
    }

    if(fpsRatio != 1.0)
    {
        foreach(SubEntry se, sf.entries)
        {
            se.frameStart = (long)floor(fpsRatio * se.frameStart);
            se.frameStop = (long)floor(fpsRatio * se.frameStop);
        }
    }

    QStringList targetLines = targetFormat->encode(sf);

    return writeFile(targetFileName, targetLines);
}

QStringList SubtitleConverter::readFile(const QString & filename, long atMostLines)
{
    QStringList buff;
    long current = 0;
    QFile inputFile(filename);
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd() || current < atMostLines)
       {
          buff += in.readLine();
          ++current;
       }
       inputFile.close();
    }
    return buff;
}

bool SubtitleConverter::writeFile(const QString & filename, const QStringList & lines)
{
    QFile outputFile(filename);
    if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&outputFile);
        foreach(QString line, lines)
        {
            out << line;
        }
        outputFile.close();
        return true;
    } else {
        return false;
    }
}

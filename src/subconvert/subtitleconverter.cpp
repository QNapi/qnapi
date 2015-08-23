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
                                         QString targetFileName,
                                         QString movieFile)
{
    return convertSubtitles(subtitleFile, targetFormatName, targetFileName, [&]() -> double {
        FFProbeMovieInfoParser mip(GlobalConfig().ffProbePath());
        MovieInfo mfm = mip.parseFile(movieFile);
        if(mfm.isFilled)
            return mfm.frameRate;
        else
            return 0.0;
    });
}

bool SubtitleConverter::convertSubtitles(QString subtitleFile,
                                         QString targetFormatName,
                                         QString targetFileName,
                                         std::function<double ()> determineFPS,
                                         double fpsRatio)
{
    QStringList subtitleLines = readFile(subtitleFile);

    QString detectedFormat = detectFormat(subtitleLines);
    if(detectedFormat.isEmpty())
        return false;

    SubtitleFormat * inputFormat = GlobalFormatsRegistry().select(detectedFormat);
    SubtitleFormat * targetFormat = GlobalFormatsRegistry().select(targetFormatName);


    SubFile sf = inputFormat->decode(subtitleLines);

    qDebug() << "ENTRIES:" << sf.entries.size();

    if(inputFormat->isTimeBased() != targetFormat->isTimeBased())
    {
        double frameRate = determineFPS();
        if(frameRate == 0.0)
            return false;

        if(targetFormat->isTimeBased())
        {
            // convert frames to timestamps
            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = (long)floor(1000.0 * sf.entries[i].frameStart / frameRate);
                sf.entries[i].frameStop = (long)floor(1000.0 * sf.entries[i].frameStop / frameRate);
            }
        }
        else
        {
            // convert timestamps to frames
            for(int i = 0; i < sf.entries.size(); ++i)
            {
                sf.entries[i].frameStart = (long)floor(frameRate * sf.entries[i].frameStart / 1000.0);
                sf.entries[i].frameStop = (long)floor(frameRate * sf.entries[i].frameStop / 1000.0);
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
            out << line << "\r\n";
        }
        outputFile.close();
        return true;
    } else {
        return false;
    }
}

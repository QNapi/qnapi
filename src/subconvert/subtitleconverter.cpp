#include "subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"
#include "qnapiconfig.h"
#include "ffprobemovieinfoparser.h"
#include "movieinfo.h"
#include "version.h"
#include <cmath>
#include <QTextStream>

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

    if(inputFormat->isTimeBased() != targetFormat->isTimeBased())
    {
        double frameRate = determineFPS();
        if(frameRate <= 0.0)
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
        for(int i = 0; i < sf.entries.size(); ++i)
        {
            sf.entries[i].frameStart = (long)floor(fpsRatio * sf.entries[i].frameStart);
            sf.entries[i].frameStop = (long)floor(fpsRatio * sf.entries[i].frameStop);
        }
    }

    if(!GlobalConfig().ppSkipConvertAds() && !sf.entries.isEmpty())
    {
        SubToken stQNapi;
        stQNapi.type = STT_WORD;
        stQNapi.payload = "QNapi";

        if(!sf.entries.last().tokens.contains(stQNapi))
        {
            SubEntry adEntry;
            if(targetFormat->isTimeBased())
            {
                adEntry.frameStart = sf.entries.back().frameStop + 2000L;
                adEntry.frameStop = adEntry.frameStart + 8000L;
            }
            else
            {
                adEntry.frameStart = sf.entries.back().frameStop + 50L;
                adEntry.frameStop = adEntry.frameStart + 200L;
            }
            QString ad = QString("Napisy pobrane i przetworzone programem QNapi|");
            adEntry.tokens = targetFormat->decodeTokenStream(ad);
            SubToken urlToken;
            urlToken.type = STT_WORD;
            urlToken.payload = QNAPI_URL;
            adEntry.tokens.push_back(urlToken);
            sf.entries.push_back(adEntry);
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

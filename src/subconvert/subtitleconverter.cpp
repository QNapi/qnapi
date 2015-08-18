#include "subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"
#include "qnapiconfig.h"
#include "ffprobemovieinfoparser.h"
#include "movieinfo.h"
#include <cmath>


QString SubtitleConverter::detectFormat(const QStringList &subtitleLines)
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

            /// TODO: fix 0 frameStop
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

    writeFile(targetFileName, targetLines);

    return true;
}

QStringList SubtitleConverter::readFile(const QString & filename)
{
    QStringList buff;
    return buff;
}

void SubtitleConverter::writeFile(const QString & filename, const QStringList & lines)
{
}

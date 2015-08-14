#ifndef SUBTITLECONVERTER_H
#define SUBTITLECONVERTER_H

#include "subconvert/subtitleformatsregistry.h"
#include "qnapiconfig.h"
#include "ffprobemovieinfoparser.h"
#include "movieinfo.h"
#include <cmath>

class SubtitleConverter
{
public:
    SubtitleConverter();

    QString detectFormat(const QStringList &subtitleLines)
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

    bool convertFormat(QString movieFile, QString subtitleFile, QString targetFormatName)
    {
        // read file to lines buffer
        QStringList subtitleLines;

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
            FFProbeMovieInfoParser mip(GlobalConfig().ffProbePath());
            MovieInfo mfm = mip.parseFile(movieFile);
            if(!mfm.isFilled)
                return false;

            if(targetFormat->isTimeBased())
            { // but input not
                // we have to convert frames to times
                foreach(SubEntry se, sf.entries)
                {
                    se.frameStart = (long)floor(1000.0 * se.frameStart / mfm.frameRate);
                    se.frameStop = (long)floor(1000.0 * se.frameStop / mfm.frameRate);
                }
            }
            else
            {
                // we have to convert times to frames
                foreach(SubEntry se, sf.entries)
                {
                    se.frameStart = (long)floor(mfm.frameRate * se.frameStart / 1000.0);
                    se.frameStop = (long)floor(mfm.frameRate * se.frameStop / 1000.0);
                }
            }
        }

        QStringList targetLines = targetFormat->encode(sf);

        // write targetLines to a file
        return true;
    }

};

#endif // SUBTITLECONVERTER_H

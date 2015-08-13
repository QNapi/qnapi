#ifndef FFPROBEMOVIEINFOPARSER_H
#define FFPROBEMOVIEINFOPARSER_H

#include <QString>
#include "movieinfo.h"

class FFProbeMovieInfoParser
{
    QString ffProbePath;
public:
    FFProbeMovieInfoParser(const QString & ffProbeExePath);

    MovieInfo parseFile(const QString & movieFilePath) const;
};

#endif // FFPROBEMOVIEINFOPARSER_H

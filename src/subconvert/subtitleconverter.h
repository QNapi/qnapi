#ifndef SUBTITLECONVERTER_H
#define SUBTITLECONVERTER_H

#include <QString>
#include <QStringList>

class SubtitleConverter
{
public:
    SubtitleConverter();

    QString detectFormat(const QStringList &subtitleLines);

    bool convertFormat(QString movieFile, QString subtitleFile, QString targetFormatName, double fpsRatio = 1.0);

};

#endif // SUBTITLECONVERTER_H

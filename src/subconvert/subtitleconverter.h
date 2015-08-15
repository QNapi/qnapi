#ifndef SUBTITLECONVERTER_H
#define SUBTITLECONVERTER_H

#include <QString>
#include <QStringList>
#include <functional>

class SubtitleConverter
{
public:
    QString detectFormat(const QStringList &subtitleLines);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetFileName,
                          double movieFPS,
                          double fpsRatio = 1.0);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetExt,
                          QString movieFile);

    bool convertSubtitles(QString subtitleFile,
                          QString targetFormatName,
                          QString targetFileName,
                          std::function<double ()> determineFPS,
                          double fpsRatio = 1.0);

private:
    QStringList readFile(const QString & filename);
    void writeFile(const QString & filename, const QStringList & lines);

};

#endif // SUBTITLECONVERTER_H

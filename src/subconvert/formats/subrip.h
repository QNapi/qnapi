#ifndef SUBRIPSUBTITLEFORMAT_H
#define SUBRIPSUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class SubRipSubtitleFormat : public SubtitleFormat
{
public:
    virtual ~SubRipSubtitleFormat() {}

    bool isTimeBased() { return true; }
    QString formatName() { return "SRT"; }
    QString defaultExtension() { return "srt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

private:
    struct SrtTimestamps
    {
        int h1;
        int m1;
        int s1;
        int ms1;
        int h2;
        int m2;
        int s2;
        int ms2;
    };

    void addEntry(QVector<SubEntry> & entries, QString & tokensBuff, SrtTimestamps & timestamps);
    QStringList encodeEntry(const SubEntry & entry, int i);
    QString encodeToken(const SubToken & entry);
};

#endif // SUBRIPSUBTITLEFORMAT_H

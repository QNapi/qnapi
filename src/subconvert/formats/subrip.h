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

};

#endif // SUBRIPSUBTITLEFORMAT_H

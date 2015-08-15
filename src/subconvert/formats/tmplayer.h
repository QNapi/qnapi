#ifndef TMPLAYERSUBTITLEFORMAT_H
#define TMPLAYERSUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class TMPlayerSubtitleFormat : public SubtitleFormat
{
public:
    virtual ~TMPlayerSubtitleFormat() {}

    bool isTimeBased() { return true; }
    QString formatName() { return "TMP"; }
    QString defaultExtension() { return "txt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

};

#endif // TMPLAYERSUBTITLEFORMAT_H

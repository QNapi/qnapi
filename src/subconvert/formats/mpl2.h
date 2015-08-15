#ifndef MPL2SUBTITLEFORMAT_H
#define MPL2SUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class MPL2SubtitleFormat : public SubtitleFormat
{
public:
    virtual ~MPL2SubtitleFormat() {}

    bool isTimeBased() { return true; }
    QString formatName() { return "MPL2"; }
    QString defaultExtension() { return "txt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

};

#endif // MPL2SUBTITLEFORMAT_H

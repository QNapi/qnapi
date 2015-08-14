#ifndef MICRODVD_H
#define MICRODVD_H

#include "subconvert/subtitleformat.h"
#include <QString>

class MicroDVDSubtitleFormat : public SubtitleFormat
{
public:
    bool isTimeBased() { return false; }
    QString formatName() { return "mDVD"; }
    QString defaultExtension() { return "txt"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);
};

#endif // MICRODVD_H


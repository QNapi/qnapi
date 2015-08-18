#ifndef MICRODVD_H
#define MICRODVD_H

#include "subconvert/subtitleformat.h"
#include <QString>

class MicroDVDSubtitleFormat : public SubtitleFormat
{
public:
    virtual ~MicroDVDSubtitleFormat() {}

    bool isTimeBased() { return false; }
    QString formatName() { return "mDVD"; }
    QString defaultExtension() { return "sub"; }

    bool detect(const QStringList &lines);
    SubFile decode(const QStringList &lines);
    QStringList encode(const SubFile & subFile);

private:
    QString encodeEntry(const SubEntry & entry);
    QString encodeToken(const SubToken & entry);
};

#endif // MICRODVD_H


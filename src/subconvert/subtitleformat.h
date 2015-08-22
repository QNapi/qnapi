#ifndef SUBTITLEFORMAT_H
#define SUBTITLEFORMAT_H

#include "subconvert/subfile.h"
#include <QString>

class SubtitleFormat
{
public:
    virtual ~SubtitleFormat() {}
    virtual bool isTimeBased() = 0;
    virtual QString formatName() = 0;
    virtual QString defaultExtension() = 0;

    virtual bool detect(const QStringList & lines) = 0;
    virtual SubFile decode(const QStringList & lines) = 0;
    virtual QStringList encode(const SubFile & subFile) = 0;

protected:
    QVector<SubToken> decodeTokenStream(QString tokenStream);

};

#endif // SUBTITLEFORMAT_H


#ifndef SUBTITLEFORMATSREGISTRY_H
#define SUBTITLEFORMATSREGISTRY_H

#include <QString>
#include <QMap>
#include <QStringList>
#include "subconvert/subtitleformat.h"

class SubtitleFormatsRegistry
{
private:
    QMap<QString, SubtitleFormat*> formats;
public:
    SubtitleFormatsRegistry();
    ~SubtitleFormatsRegistry();

    void registerFormat(SubtitleFormat *format);
    QStringList enumerateFormats() const;
    SubtitleFormat* select(const QString & format) const;

};

SubtitleFormatsRegistry & GlobalFormatsRegistry();

#endif // SUBTITLEFORMATSREGISTRY_H

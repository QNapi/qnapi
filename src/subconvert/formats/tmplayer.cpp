#include "tmplayer.h"


bool TMPlayerSubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile TMPlayerSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList TMPlayerSubtitleFormat::encode(const SubFile & subFile)
{
    QStringList out;
    return out;
}

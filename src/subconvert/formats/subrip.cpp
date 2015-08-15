#include "subrip.h"


bool SubRipSubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile SubRipSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList SubRipSubtitleFormat::encode(const SubFile & subFile)
{
    QStringList out;
    return out;
}


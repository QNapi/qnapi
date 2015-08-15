#include "mpl2.h"

bool MPL2SubtitleFormat::detect(const QStringList &lines)
{
    return false;
}

SubFile MPL2SubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    return sf;
}

QStringList MPL2SubtitleFormat::encode(const SubFile & subFile)
{
    QStringList out;
    return out;
}

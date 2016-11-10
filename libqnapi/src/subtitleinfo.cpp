#include "subtitleinfo.h"
#include <algorithm>
#include <vector>
#include <QFileInfo>

bool SubtitleInfo::operator<(const SubtitleInfo & other) const
{
    std::vector<QString> v1 = { QString::number(other.resolution), name };
    std::vector<QString> v2 = { QString::number(resolution), other.name };

    return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
}

SubtitleInfo SubtitleInfo::fromFailed(const QString & fileName)
{
    return SubtitleInfo("", "", "", QFileInfo(fileName).completeBaseName(), "" ,"", SUBTITLE_NONE);
}

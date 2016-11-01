#include "qnapisubtitleinfo.h"
#include <algorithm>
#include <vector>
#include <QFileInfo>

bool QNapiSubtitleInfo::operator<(const QNapiSubtitleInfo& other) const
{
    std::vector<QString> v1 = { QString::number(other.resolution), name };
    std::vector<QString> v2 = { QString::number(resolution), other.name };

    return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
}

QNapiSubtitleInfo QNapiSubtitleInfo::fromFailed(QString fileName)
{
    return QNapiSubtitleInfo("", "", "", QFileInfo(fileName).completeBaseName(), "" ,"", SUBTITLE_NONE);
}

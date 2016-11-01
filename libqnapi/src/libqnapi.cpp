#include "libqnapi.h"
#include "version.h"
#include "movieinfo/libmediainfomovieinfoprovider.h"

LibQNapi::LibQNapi()
{
}

QString LibQNapi::version()
{
    return QString(QNAPI_VERSION);
}

QString LibQNapi::displayableVersion()
{
    return QString(QNAPI_DISPLAYABLE_VERSION);
}

QString LibQNapi::webpageUrl()
{
    return QString(QNAPI_URL);
}

QSharedPointer<const MovieInfoProvider> LibQNapi::movieInfoProvider()
{
    return QSharedPointer<const MovieInfoProvider>(new LibmediainfoMovieInfoProvider());
}

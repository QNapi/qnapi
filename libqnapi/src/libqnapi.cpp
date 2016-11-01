#include "libqnapi.h"
#include "version.h"

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

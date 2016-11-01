#ifndef LIBQNAPI_H
#define LIBQNAPI_H

#include "movieinfo/movieinfoprovider.h"
#include <QString>
#include <QSharedPointer>

class LibQNapi
{

public:
    LibQNapi();

    static QString version();
    static QString displayableVersion();
    static QString webpageUrl();
    static QSharedPointer<const MovieInfoProvider> movieInfoProvider();
};

#endif // LIBQNAPI_H

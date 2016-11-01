#ifndef LIBQNAPI_H
#define LIBQNAPI_H

#include <QString>

class LibQNapi
{

public:
    LibQNapi();

    static QString version();
    static QString displayableVersion();
    static QString webpageUrl();
};

#endif // LIBQNAPI_H

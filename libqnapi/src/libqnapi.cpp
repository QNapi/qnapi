/*****************************************************************************
** QNapi
** Copyright (C) 2008-2016 Piotr Krzemiński <pio.krzeminski@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#include "libqnapi.h"
#include "version.h"
#include "movieinfo/libmediainfomovieinfoprovider.h"

#include <QFileInfo>
#include <QDir>

QString LibQNapi::appExecutablePath = "";

void LibQNapi::init(const QString & appExecutablePath)
{
    LibQNapi::appExecutablePath = appExecutablePath;
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

QSharedPointer<const StaticConfig>
LibQNapi::staticConfig()
{
    return QSharedPointer<const StaticConfig>(new StaticConfig);
}

QSharedPointer<const ConfigReader>
LibQNapi::configReader()
{
    return QSharedPointer<const ConfigReader>(
        new ConfigReader(staticConfig(), subtitleDownloadEngineRegistry())
    );
}

QSharedPointer<const ConfigWriter>
LibQNapi::configWriter()
{
    QString ver = version();
    return QSharedPointer<const ConfigWriter>(new ConfigWriter(ver));
}

const QNapiConfig LibQNapi::loadConfig()
{
    if(LibQNapi::isPortableMode())
        return LibQNapi::configReader()->readPortableConfig(LibQNapi::portableConfigPath());
    else
        return LibQNapi::configReader()->readUserConfig();
}

void LibQNapi::writeConfig(const QNapiConfig & config)
{
    if(LibQNapi::isPortableMode())
        LibQNapi::configWriter()->writePortableConfig(LibQNapi::portableConfigPath(), config);
    else
        LibQNapi::configWriter()->writeUserConfig(config);
}

bool LibQNapi::isPortableMode()
{
    return QFileInfo(portableConfigPath()).exists();
}

QString LibQNapi::portableConfigPath()
{
    QString appExecutableDir = QFileInfo(appExecutablePath).absoluteDir().path();
    return appExecutableDir + QDir::separator() + "qnapi.ini";
}

QSharedPointer<const SubtitleDownloadEnginesRegistry>
LibQNapi::subtitleDownloadEngineRegistry()
{
    return QSharedPointer<const SubtitleDownloadEnginesRegistry>(
        new SubtitleDownloadEnginesRegistry(
            LibQNapi::displayableVersion(),
            LibQNapi::staticConfig()->subtitleExtensions(),
            &LibQNapi::p7zipDecoder
        )
    );
}

QSharedPointer<const MovieInfoProvider>
LibQNapi::movieInfoProvider()
{
    return QSharedPointer<const MovieInfoProvider>(new LibmediainfoMovieInfoProvider);
}

QSharedPointer<const SubtitleConverter>
LibQNapi::subtitleConverter(const PostProcessingConfig & ppConfig)
{
    return QSharedPointer<const SubtitleConverter>(
        new SubtitleConverter(
            LibQNapi::subtitleFormatsRegistry(),
            LibQNapi::movieInfoProvider(),
            ppConfig.skipConvertAds()
        )
    );
}

QSharedPointer<const SubtitleFormatsRegistry>
LibQNapi::subtitleFormatsRegistry()
{
    return QSharedPointer<SubtitleFormatsRegistry>(new SubtitleFormatsRegistry);
}

QSharedPointer<const P7ZipDecoder> LibQNapi::p7zipDecoder(const QString & p7zipPath)
{
    return QSharedPointer<const P7ZipDecoder>(new P7ZipDecoder(p7zipPath));
}

QSharedPointer<const SubtitleMatcher> LibQNapi::subtitleMatcher(const QNapiConfig & config)
{
    return QSharedPointer<const SubtitleMatcher>(
        new SubtitleMatcher(
            config.generalConfig().noBackup(),
            config.postProcessingConfig().enabled(),
            config.postProcessingConfig().subFormat(),
            config.postProcessingConfig().subExtension(),
            config.generalConfig().changePermissionsEnabled(),
            config.generalConfig().changePermissionsTo(),
            LibQNapi::subtitleFormatsRegistry()
        )
    );
}

QSharedPointer<const SubtitlePostProcessor> LibQNapi::subtitlePostProcessor(const PostProcessingConfig & config)
{
    return QSharedPointer<const SubtitlePostProcessor>(
        new SubtitlePostProcessor(
            config,
            LibQNapi::subtitleConverter(config)
        )
    );
}

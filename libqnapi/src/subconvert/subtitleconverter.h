/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#ifndef SUBTITLECONVERTER_H
#define SUBTITLECONVERTER_H

#include "movieinfo/movieinfoprovider.h"
#include "subconvert/subtitleformatsregistry.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <functional>

class SubtitleConverter {
 public:
  SubtitleConverter(
      QSharedPointer<const SubtitleFormatsRegistry> subtitleFormatsRegistry,
      QSharedPointer<const MovieInfoProvider> movieInfoProvider,
      bool skipConvertAds);

  QString detectFormat(const QString &subtitleFile) const;
  QString detectFormat(const QStringList &subtitleLines) const;

  bool convertSubtitles(QString subtitleFile, QString targetFormatName,
                        QString targetFileName, double movieFPS,
                        double fpsRatio, double delayOffset) const;

  bool convertSubtitles(QString subtitleFile, QString targetFormatName,
                        QString targetFileName, QString movieFile) const;

  bool convertSubtitles(QString subtitleFile, QString targetFormatName,
                        QString targetFileName,
                        std::function<double()> determineFPS,
                        double fpsRatio = 1.0, double delayOffset = 0.0) const;

 private:
  QSharedPointer<const SubtitleFormatsRegistry> subtitleFormatsRegistry;
  QSharedPointer<const MovieInfoProvider> movieInfoProvider;
  bool skipConvertAds;

  long ts2frame(long ts, double frameRate) const;
  long frame2ts(long frame, double frameRate) const;

  QStringList readFile(const QString &filename, QString encoding,
                       long atMostLines = 0) const;
  bool writeFile(const QString &filename, QString encoding,
                 const QStringList &lines) const;
};

#endif  // SUBTITLECONVERTER_H

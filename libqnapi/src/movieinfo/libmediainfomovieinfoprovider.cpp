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

#include "libmediainfomovieinfoprovider.h"
#include <QRegExp>
#include <string>

#ifndef UNICODE
#define UNICODE
#endif

#ifdef UNICODE
#define _UNICODE
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
#include "MediaInfoDLL/MediaInfoDLL.h"  //Dynamicly-loaded library (.dll or .so)
#define MediaInfoNameSpace MediaInfoDLL;
#else
#include "MediaInfo/MediaInfo.h"  //Staticly-loaded library (.lib or .a or .so)
#define MediaInfoNameSpace MediaInfoLib;
#endif
using namespace MediaInfoNameSpace;

const Maybe<MovieInfo> LibmediainfoMovieInfoProvider::getMovieInfo(
    const QString& moviePath) const {
  MediaInfo mi;
  mi.Option(__T("Internet"), __T("No"));

  mi.Open(moviePath.toStdWString());

#define GET_VIDEO_INFO(__streamIdx, __key) \
  QString::fromStdWString(                 \
      std::wstring(mi.Get(Stream_Video, __streamIdx, __T(__key), Info_Text)))

  QString widthS = GET_VIDEO_INFO(0, "Width");
  QString heightS = GET_VIDEO_INFO(0, "Height");
  QString frameRateS = GET_VIDEO_INFO(0, "FrameRate");
  QString durationS = GET_VIDEO_INFO(0, "Duration");

  mi.Close();

  QRegExp rWidth("(\\d+)");
  if (rWidth.indexIn(widthS) == -1) return nothing();

  QRegExp rHeight("(\\d+)");
  if (rHeight.indexIn(heightS) == -1) return nothing();

  QRegExp rFrameRate("(\\d+).(\\d+)");
  if (rFrameRate.indexIn(frameRateS) == -1) return nothing();
  long frFloor = rFrameRate.cap(1).toLong();
  long frFrac = rFrameRate.cap(2).toLong();

  QRegExp rDuration("(\\d+)");
  if (rDuration.indexIn(durationS) == -1) return nothing();

  MovieInfo info(rWidth.cap(1).toInt(), rHeight.cap(1).toInt(),
                 (double)frFloor + (double)frFrac / 1000.0,
                 rDuration.cap(1).toDouble() / 1000.0);

  return just<MovieInfo>(info);
}

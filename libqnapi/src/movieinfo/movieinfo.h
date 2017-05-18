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

#ifndef MOVIEINFO_H
#define MOVIEINFO_H

#include <QMetaType>
#include <QObject>

class MovieInfo {
 private:
  int width_;
  int height_;
  double frameRate_;
  double durationSecs_;

 public:
  MovieInfo(const int& width, const int& height, const double& frameRate,
            const double& durationSecs)
      : width_(width),
        height_(height),
        frameRate_(frameRate),
        durationSecs_(durationSecs) {}

  MovieInfo() : width_(0), height_(0), frameRate_(0.0), durationSecs_(0.0) {}

  virtual ~MovieInfo() {}

  int width() const { return width_; }
  int height() const { return height_; }
  double frameRate() const { return frameRate_; }
  double durationSecs() const { return durationSecs_; }

  const MovieInfo setWidth(const int& width) const {
    return MovieInfo(width, height_, frameRate_, durationSecs_);
  }
  const MovieInfo setHeight(const int& height) const {
    return MovieInfo(width_, height, frameRate_, durationSecs_);
  }
  const MovieInfo setFrameRate(const double& frameRate) const {
    return MovieInfo(width_, height_, frameRate, durationSecs_);
  }
  const MovieInfo setDurationSecs(const double& durationSecs) const {
    return MovieInfo(width_, height_, frameRate_, durationSecs);
  }
};

Q_DECLARE_METATYPE(MovieInfo);

#endif

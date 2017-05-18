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

#ifndef MOVIEINFOPROVIDER_H
#define MOVIEINFOPROVIDER_H

#include <Maybe.h>
#include <QString>
#include "movieinfo/movieinfo.h"

class MovieInfoProvider {
 public:
  virtual ~MovieInfoProvider() {}
  virtual const Maybe<MovieInfo> getMovieInfo(
      const QString& moviePath) const = 0;
};

#endif  // MOVIEINFOPROVIDER_H

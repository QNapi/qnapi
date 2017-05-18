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

#include "subtitleinfo.h"
#include <QFileInfo>
#include <algorithm>
#include <vector>

bool SubtitleInfo::operator<(const SubtitleInfo& other) const {
  std::vector<QString> v1 = {QString::number(other.resolution), name};
  std::vector<QString> v2 = {QString::number(resolution), other.name};

  return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(),
                                      v2.end());
}

SubtitleInfo SubtitleInfo::fromFailed(const QString& fileName) {
  return SubtitleInfo("", "", "", QFileInfo(fileName).completeBaseName(), "",
                      "", SUBTITLE_NONE);
}

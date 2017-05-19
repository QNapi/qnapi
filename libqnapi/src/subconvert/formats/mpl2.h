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

#ifndef MPL2SUBTITLEFORMAT_H
#define MPL2SUBTITLEFORMAT_H

#include "subconvert/subtitleformat.h"

class MPL2SubtitleFormat : public SubtitleFormat {
 public:
  virtual ~MPL2SubtitleFormat() {}

  bool isTimeBased() const { return true; }
  QString formatName() const { return "MPL2"; }
  QString defaultExtension() const { return "txt"; }

  bool detect(const QStringList &lines) const;
  SubFile decode(const QStringList &lines) const;
  QStringList encode(const SubFile &subFile) const;

 private:
  QString encodeEntry(const SubEntry &entry) const;
  QString encodeToken(const SubToken &entry) const;
};

#endif  // MPL2SUBTITLEFORMAT_H

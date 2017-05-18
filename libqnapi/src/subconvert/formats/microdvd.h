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

#ifndef MICRODVD_H
#define MICRODVD_H

#include <QString>
#include "subconvert/subtitleformat.h"

class MicroDVDSubtitleFormat : public SubtitleFormat {
 public:
  virtual ~MicroDVDSubtitleFormat() {}

  bool isTimeBased() const { return false; }
  QString formatName() const { return "mDVD"; }
  QString defaultExtension() const { return "sub"; }

  bool detect(const QStringList &lines) const;
  SubFile decode(const QStringList &lines) const;
  QStringList encode(const SubFile &subFile) const;

 private:
  QString encodeEntry(const SubEntry &entry) const;
  QString encodeToken(const SubToken &entry) const;
};

#endif  // MICRODVD_H

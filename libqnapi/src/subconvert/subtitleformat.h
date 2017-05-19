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

#ifndef SUBTITLEFORMAT_H
#define SUBTITLEFORMAT_H

#include <QString>
#include "subconvert/subfile.h"

class SubtitleFormat {
 public:
  virtual ~SubtitleFormat() {}
  virtual bool isTimeBased() const = 0;
  virtual QString formatName() const = 0;
  virtual QString defaultExtension() const = 0;

  virtual bool detect(const QStringList& lines) const = 0;
  virtual SubFile decode(const QStringList& lines) const = 0;
  virtual QStringList encode(const SubFile& subFile) const = 0;

  QVector<SubToken> decodeTokenStream(QString tokenStream) const;

 private:
  QString parseColor(QString colorString) const;
};

#endif  // SUBTITLEFORMAT_H

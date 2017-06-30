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

#ifndef DONTSHOWLISTARGPARSER_H
#define DONTSHOWLISTARGPARSER_H

#include <QCoreApplication>
#include "parser/cliargparser.h"

class DontShowListArgParser : public CliArgParser {
  Q_DECLARE_TR_FUNCTIONS()

 public:
  DontShowListArgParser();

  virtual QVariant parse(const QStringList& args,
                         const QNapiConfig& config) const;

  virtual Maybe<HelpInfo> helpInfo() const;
};

#endif  // DONTSHOWLISTARGPARSER_H

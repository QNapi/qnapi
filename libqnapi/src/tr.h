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

#ifndef NAMESPACE_TR_H
#define NAMESPACE_TR_H

#include <QCoreApplication>

#define Q_DECLARE_NAMESPACE_TR(context)                                      \
  inline QString tr(const char *sourceText,                                  \
                    const char *disambiguation = Q_NULLPTR, int n = -1) {    \
    return QCoreApplication::translate(#context, sourceText, disambiguation, \
                                       n);                                   \
  }

#define Q_DECLARE_CLASS_TR(context)                                          \
  inline QString tr(const char *sourceText,                                  \
                    const char *disambiguation = Q_NULLPTR, int n = -1)      \
      const {                                                                \
    return QCoreApplication::translate(#context, sourceText, disambiguation, \
                                       n);                                   \
  }

#endif  // NAMESPACE_TR_H

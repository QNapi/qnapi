/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
**
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QMANAGED_REQUEST_H_
#define _QMANAGED_REQUEST_H_

#include "qcumber.h"

/*!
    \file qmanagedrequest.h

    \brief Definition of the QManagedRequest class.
*/

#include <QStringList>

class QCUMBER_EXPORT QManagedRequest {
 public:
  QManagedRequest(const QString& cmd, const QStringList& args = QStringList());
  QManagedRequest(const QManagedRequest& r);
  ~QManagedRequest();

  static QManagedRequest createSystemRequest(const QStringList& args,
                                             int id = -1);

  QManagedRequest& operator=(const QManagedRequest& r);

  QString toString() const;
  static QManagedRequest fromString(const QString& s);

  static QStringList splitArguments(const QString& s);
  static QByteArray joinArguments(const QString& cmd, const QStringList& l);

 private:
  QString sCommand;
  QStringList lArguments;
};

#endif  // _QMANAGED_REQUEST_H_

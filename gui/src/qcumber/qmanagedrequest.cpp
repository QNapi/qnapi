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

#include "qmanagedrequest.h"

/*!
    \file qmanagedrequest.cpp

    \brief Implementation of the QManagedRequest class.
*/

QManagedRequest::QManagedRequest(const QString& cmd, const QStringList& args)
    : sCommand(cmd), lArguments(args) {}

QManagedRequest::QManagedRequest(const QManagedRequest& r)
    : sCommand(r.sCommand), lArguments(r.lArguments) {}

QManagedRequest::~QManagedRequest() {
  sCommand.clear();
  lArguments.clear();
}

QManagedRequest QManagedRequest::createSystemRequest(const QStringList& args,
                                                     int id) {
  QManagedRequest r("--request");
  r.lArguments = args;
  r.lArguments.prepend(QString::number(id));

  return r;
}

QManagedRequest& QManagedRequest::operator=(const QManagedRequest& r) {
  sCommand = r.sCommand;
  lArguments = r.lArguments;

  return *this;
}

QString QManagedRequest::toString() const {
  return QString::fromUtf8(joinArguments(sCommand, lArguments));
}

QManagedRequest QManagedRequest::fromString(const QString& s) {
  QManagedRequest r("");
  r.lArguments = splitArguments(s);
  r.sCommand = r.lArguments.takeAt(0);

  return r;
}

QStringList QManagedRequest::splitArguments(const QString& s) {
  int i = -1;
  QStringList d, l = s.split(QRegExp("\\s"));

  while (++i < l.count()) {
    if (l.at(i).startsWith("\"")) {
      QString arg;

      do {
        arg += " ";
        arg += l.at(i);
      } while (!arg.endsWith("\"") && (++i < l.count()));

      arg.remove(0, 2);

      if (arg.endsWith("\"")) arg.chop(1);

      d << arg;
    } else {
      d << l.at(i);
    }
  }

  return d;
}

QByteArray QManagedRequest::joinArguments(const QString& cmd,
                                          const QStringList& l) {
  QByteArray msg;

  msg += cmd.toUtf8();

  foreach (QString a, l) {
    if (a.isEmpty()) continue;

    if (a.contains(QRegExp("\\s"))) a = "\"" + a + "\"";

    msg += " ";
    msg += a.toUtf8();
  }

  return msg;
}

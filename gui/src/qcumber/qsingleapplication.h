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

#ifndef _QSINGLE_APPLICATION_H_
#define _QSINGLE_APPLICATION_H_

#include "qcumber.h"

/*!
    \file qsingleapplication.h

    \brief Definition of the QSingleApplication class.
*/

#include <QApplication>

class QInterProcessChannel;

class QCUMBER_EXPORT QSingleApplication : public QApplication {
  Q_OBJECT

 public:
  enum InstanciationPolicy { None, ForwardArguments };

  enum MessagingPolicy { Ignore, Events, Signals };

  QSingleApplication(int& argc, char** argv, const QString& appName);
  virtual ~QSingleApplication();

  bool isInstanceAllowed() const;

  MessagingPolicy messagingPolicy() const;
  void setMessagingPolicy(MessagingPolicy p);

  InstanciationPolicy instanciationPolicy() const;
  void setInstanciationPolicy(InstanciationPolicy p);

 public slots:
  virtual int exec();

  void sendRequest(const QString& s);
  void sendRequest(const QStringList& l);

 signals:
  void request(const QString& s);

 protected:
  virtual bool event(QEvent* e);

 protected slots:
  virtual void message(const QString& msg);
  virtual void request(const QStringList& r);

 private:
  QInterProcessChannel* pChannel;

  MessagingPolicy m_messaging;
  InstanciationPolicy m_instanciation;
};

#endif  // !_QSINGLE_APPLICATION_H_

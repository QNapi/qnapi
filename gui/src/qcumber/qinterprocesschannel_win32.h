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

#ifndef _QINTER_PROCESS_CHANNEL_H_
#define _QINTER_PROCESS_CHANNEL_H_

#include "qcumber.h"
#include "qmanagedrequest.h"

#include <QApplication>
#include <QByteArray>
#include <QString>
#include <QThread>

#include <qt_windows.h>

/*!
    \file qinterprocesschannel.h

    \brief Definition of the QInterProcessChannel class.
*/

// buffer size for input/output pipes
static const int bufferSize = 2048;

class QCUMBER_EXPORT QInterProcessChannel : public QThread {
  Q_OBJECT

 public:
  QInterProcessChannel(QObject* p = 0);
  virtual ~QInterProcessChannel();

  bool isServer() const;

  QString messageBuffer() const;

 public slots:
  void close();

  void sendMessage();
  void sendMessage(const QString& s);
  void sendMessage(const QByteArray& b);

  void setMessageBuffer(const QString& s);

 signals:
  void message(const QString& s);
  void request(const QStringList& l);

 protected:
  virtual void run();

 private slots:
  void init();

 private:
  QString uniqPrefix, pipeName, globalMutexStr, blockerMutexStr;
  QString sMsg;
  bool serverMode;
};

#endif  // _QINTER_PROCESS_CHANNEL_H_

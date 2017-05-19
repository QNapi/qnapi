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

#include "qinterprocesschannel_win32.h"

/*!
    \class QInterProcessChannel

    \brief A generic network-based communication channel for applications.

    QInterProcessChannel offers communication facilities between several
   instances of the same application. It initializes itself so that the first
   created instance acts as a "server" which recieve messages from the others
   instances. This is particularly handy when creating a single-instance
   application which, for example, uses file association through a command line
   interface.

    \see QSingleApplication
*/

/*!
    \brief Constructor

    Check for a valid server and create one if none found.
*/
QInterProcessChannel::QInterProcessChannel(QObject* p) : QThread(p) {
  uniqPrefix = QApplication::applicationName();
  pipeName =
      QString("\\\\.\\pipe\\%1%2").arg(uniqPrefix).arg("_uniquePipeName");
  globalMutexStr = uniqPrefix + QString("_globalMutexId");
  blockerMutexStr = uniqPrefix + QString("_blockerMutexId");
  init();
}

/*!
    \brief Destructor
*/
QInterProcessChannel::~QInterProcessChannel() {
  // while(isRunning())
  //  quit();

  close();
}

/*!
    \return Wether the instance has a server role
*/
bool QInterProcessChannel::isServer() const { return serverMode; }

/*!
    \return The current buffered message
*/
QString QInterProcessChannel::messageBuffer() const { return sMsg; }

/*!
    \brief Sets a buffered message
*/
void QInterProcessChannel::setMessageBuffer(const QString& s) { sMsg = s; }

/*!
    \overload

    Sends the content of the current message buffer
*/
void QInterProcessChannel::sendMessage() {
  sendMessage(sMsg);
  sMsg.clear();
}

/*!
    \overload
*/
void QInterProcessChannel::sendMessage(const QString& s) {
  sendMessage(s.toUtf8());
}
/*!
    \brief Send a message to server instance
*/
void QInterProcessChannel::sendMessage(const QByteArray& msg) {
  if (msg.count()) {
    // a lot of simultaneous clients cause that pipe sometimes omits
    // some of these clients despite results of writing functions are ok ;/
    // workaround: reserve mutex, wait 100ms before every write operation

    HANDLE mutex = CreateMutexA(NULL, FALSE, qPrintable(blockerMutexStr));
    WaitForSingleObject(mutex, INFINITE);
    Sleep(100);

    HANDLE hPipe;

    // loop to get pipe's handle
    for (;;) {
      hPipe = CreateFileA(qPrintable(pipeName), GENERIC_WRITE, 0, NULL,
                          OPEN_EXISTING, 0, NULL);

      if (hPipe != INVALID_HANDLE_VALUE) break;

      if (GetLastError() != ERROR_PIPE_BUSY) {
        ReleaseMutex(mutex);
        return;
      }

      // wait max 1s and retry
      WaitNamedPipeA(qPrintable(pipeName), 1000);
    }

    // ok, now we have pipe's handle, we can prepare buffers and write to the
    // pipe

    char buffer[bufferSize];
    strncpy(buffer, msg.constData(), bufferSize);
    buffer[bufferSize - 1] = 0;

    DWORD bytesWritten;
    bool writeSuccess =
        WriteFile(hPipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL);

    if (!writeSuccess) {
      ReleaseMutex(mutex);
      return;
    }

    CloseHandle(hPipe);
    ReleaseMutex(mutex);
  } else {
    // qWarning("::sendMessage(%d): Empty messages are not carried out...",
    // GetCurrentProcessId());
  }
}

/*!
    \brief Closes the communication channel
*/
void QInterProcessChannel::close() {
  if (serverMode) {
    // if we want to quit pipe server thread, we have to send --quit message
    sendMessage(QString("--quit"));
  }
}

/*!
    \internal
*/
void QInterProcessChannel::run() {
  if (!serverMode) return;

  HANDLE hPipe;
  char buffer[bufferSize];
  bool connected, readSuccess;
  DWORD bytesRead;

  // thread loop
  for (;;) {
    hPipe =
        CreateNamedPipeA(qPrintable(pipeName), PIPE_ACCESS_INBOUND,
                         PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                         1, 0, bufferSize, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) break;

    connected = ConnectNamedPipe(hPipe, NULL)
                    ? TRUE
                    : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected) {
      CloseHandle(hPipe);
      continue;
    }

    readSuccess = ReadFile(hPipe, buffer, bufferSize, &bytesRead, NULL);
    buffer[bufferSize - 1] = 0;

    if (!readSuccess || !bytesRead) {
      CloseHandle(hPipe);
      continue;
    }

    QStringList argv = QManagedRequest::splitArguments(QString(buffer));
    const QString cmd = argv.at(0);

    // if server instance is going to suspend thread...
    if (cmd == "--quit") {
      CloseHandle(hPipe);
      break;
    } else if (cmd == "--request") {
      argv.removeAt(0);
      emit request(argv);
    } else {
      emit message(QString(buffer));
    }

    CloseHandle(hPipe);
  }
}

/*!
    \internal
*/
void QInterProcessChannel::init() {
  // while(isRunning())
  //  quit();

  // create global mutex to check single instance
  CreateMutexA(NULL, FALSE, qPrintable(globalMutexStr));
  serverMode = !(GetLastError() == ERROR_ALREADY_EXISTS);

  // start listening thread in single instance mode
  if (serverMode) start();
}

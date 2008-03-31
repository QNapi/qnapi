/****************************************************************************
**
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
**
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qinterprocesschannel_win32.h"


/*!
	\class QInterProcessChannel
	
	\brief A generic network-based communication channel for applications.
	
	QInterProcessChannel offers communication facilities between several instances of
	the same application. It initializes itself so that the first created instance acts
	as a "server" which recieve messages from the others instances. This is particularly
	handy when creating a single-instance application which, for example, uses file
	association through a command line interface.
	
	\see QSingleApplication
*/

/*!
	\brief Constructor
	
	Check for a valid server and create one if none found.
*/
QInterProcessChannel::QInterProcessChannel(QObject *p)
 : QThread(p)
{
	uniqPrefix = QApplication::applicationName();
	pipeName = QString("\\\\.\\pipe\\%1%2").arg(uniqPrefix).arg("_uniquePipeName");
	globalMutexStr = uniqPrefix + QString("_globalMutexId");
	blockerMutexStr = uniqPrefix + QString("_blockerMutexId");
	init();
}

/*!
	\brief Destructor
*/
QInterProcessChannel::~QInterProcessChannel()
{
	//while(isRunning())
	//	quit();
	
	close();
}

/*!
	\return Wether the instance has a server role
*/
bool QInterProcessChannel::isServer() const
{
	return serverMode;
}

/*!
	\return The current buffered message
*/
QString QInterProcessChannel::messageBuffer() const
{
	return sMsg;
}

/*!
	\brief Sets a buffered message
*/
void QInterProcessChannel::setMessageBuffer(const QString& s)
{
	sMsg = s;
}

/*!
	\overload
	
	Sends the content of the current message buffer
*/
void QInterProcessChannel::sendMessage()
{
	sendMessage(sMsg);
	sMsg.clear();
}

/*!
	\overload
*/
void QInterProcessChannel::sendMessage(const QString& s)
{
	sendMessage(s.toLocal8Bit());
}
/*!
	\brief Send a message to server instance
*/
void QInterProcessChannel::sendMessage(const QByteArray& msg)
{
	if(msg.count())
	{
		//qDebug("::sendMessage(%d): %s", GetCurrentProcessId(), msg.constData());
		
		// workaround: reserve mutex and wait 100ms
		HANDLE mutex = CreateMutexA(NULL, FALSE, qPrintable(blockerMutexStr));
		WaitForSingleObject(mutex, INFINITE);
		Sleep(100);
		
		HANDLE hPipe;
		
		for(;;)
		{
			hPipe = CreateFileA( 
						qPrintable(pipeName),
						GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
			
			if(hPipe != INVALID_HANDLE_VALUE)
			{
				//qDebug("::sendMessage(%d): uchwyt niepoprawny, czyli OK :D", GetCurrentProcessId());
				break;
			}
			
			if(GetLastError() != ERROR_PIPE_BUSY)
			{
				//qDebug("::sendMessage(%d): inny blad przy uchwytywaniu rurki! GetLastError()=%d", GetCurrentProcessId(), GetLastError());
				ReleaseMutex(mutex);
				return;
			}
			
			//qDebug("::sendMessage(%d): czekamy max 1s", GetCurrentProcessId());
			if(!WaitNamedPipeA(qPrintable(pipeName), 1000))
			{
				//qDebug("::sendMessage(%d): timeout 1s przerkoczony!!!", GetCurrentProcessId());
				//return;
			}
			//qDebug("::sendMessage(%d): doczekalismy sie na rurke", GetCurrentProcessId());
		}
		
		char buffer[bufferSize];
		strncpy(buffer, msg.constData(), bufferSize);
		buffer[bufferSize-1] = 0;
		
		DWORD bytesWritten;
		bool writeSuccess = WriteFile( 
								hPipe,
								buffer,
								strlen(buffer) + 1,
								&bytesWritten,
								NULL);
		
		if (!writeSuccess)
		{
			//qDebug("::sendMessage(%d): WriteFile failed", GetCurrentProcessId());
			ReleaseMutex(mutex);
			return;
		}
		
		//qDebug("::sendMessage(%d): ihaaa! zapisano %d bajtów do rurki :)", GetCurrentProcessId(), bytesWritten);
		CloseHandle(hPipe);
		ReleaseMutex(mutex);
	} else {
		//qWarning("::sendMessage(%d): Empty messages are not carried out...", GetCurrentProcessId());
	}
}

/*!
	\brief Closes the communication channel
*/
void QInterProcessChannel::close()
{
	//qDebug("QInterProcessChannel::close(%d)", GetCurrentProcessId());

	if(serverMode)
	{
		//qDebug("QInterProcessChannel::close(%d) serverMode, sending QUIT", GetCurrentProcessId());
		sendMessage(QString("--quit"));
	}
}

/*!
	\internal
*/
void QInterProcessChannel::run()
{
	//qDebug("thread: run");

	if(!serverMode) return;
	
	HANDLE hPipe;
	char buffer[bufferSize];
	bool connected, readSuccess;
	DWORD bytesRead;
	
	for(;;)
	{
		hPipe = CreateNamedPipeA(
					qPrintable(pipeName),
					PIPE_ACCESS_INBOUND,
					PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
					1,
					0,
					bufferSize,
					0,
					NULL);
		
		//qDebug("thread: hPipe=%d, pipeName=%s", hPipe, qPrintable(pipeName));
		
		if(hPipe == INVALID_HANDLE_VALUE)
		{
			//qDebug("thread: CreateNamedPipe failed. continuing");
			break;
		}
		
		connected = ConnectNamedPipe(hPipe, NULL) ?
						TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		if(!connected)
		{
			//qDebug("thread: pipe not connected!!!");
			CloseHandle(hPipe);
			continue;
		}
		
		readSuccess = ReadFile(
						hPipe,
						buffer,
						bufferSize,
						&bytesRead,
						NULL);
		buffer[bufferSize - 1] = 0;
		
		//qDebug("thread: readSuccess=%d bytesRead=%d", readSuccess, bytesRead);
		
		if(!readSuccess || !bytesRead)
		{
			CloseHandle(hPipe);
			continue;
		}
		
		//qDebug("thread: buffer=%s", buffer);
		
		QStringList argv = QManagedRequest::splitArguments(QString(buffer));
		const QString cmd = argv.at(0);
		
		if ( cmd == "--quit" ) {
			CloseHandle(hPipe);
			break;
		} else if ( cmd == "--request" ) {
			argv.removeAt(0);
			emit request(argv);
		} else {
			emit message(QString(buffer));
		}
		
		CloseHandle(hPipe);
	}
	
	//qDebug("thread: end of run");
}

/*!
	\internal
*/
void QInterProcessChannel::init()
{
	//while(isRunning())
	//	quit();
	
	CreateMutexA(NULL, FALSE, qPrintable(globalMutexStr));
	serverMode = !(GetLastError() == ERROR_ALREADY_EXISTS);
	
	//qDebug("init: pid=%d, serverMode = %d", GetCurrentProcessId(), serverMode);
	
	if(serverMode) start();
}

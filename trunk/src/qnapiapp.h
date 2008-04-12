/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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

#ifndef __QNAPIAPP__H__
#define __QNAPIAPP__H__

#include <QApplication>
#include <QString>
#include <QEvent>
#include <QFileOpenEvent>

#include "qcumber/qsingleapplication.h"

class QNapiApp : public QSingleApplication
{
	Q_OBJECT
	public:
		QNapiApp(int argc, char **argv) : QSingleApplication(argc, argv) {}

	signals:
		void downloadFile(const QString & fileName);

	private:
		bool event(QEvent *ev);
};

#endif

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

#ifndef __QNAPISUBTITLEINFO__H__
#define __QNAPISUBTITLEINFO__H__

// struktura opisujaca napisy
struct QNapiSubtitleInfo
{
	QNapiSubtitleInfo(QString l, QString e, QString u, QString n = "",
					  QString c = "", QString f = "")
		: lang(l), engine(e), url(u), name(n), comment(c), format(f) {}
	QString lang;
	QString engine;
	QString url;
	QString name;
	QString comment;
	QString format;
};

#endif

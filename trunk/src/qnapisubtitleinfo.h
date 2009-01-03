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

// Orientacyjne okreslenie czy napisy na pewno pasuja lub nie do naszego filmu
enum QNapiSubtitleResolution
{
	// nie wiadomo (lub nie jestesmy pewni)
	SUBTITLE_UNKNOWN,
	// podejrzenie, ze napisy nie pasuja
	// (np. zostaly oznaczone jako nieprawidlowe)
	SUBTITLE_BAD,
	// napisy prawdopodobnie pasuja (np. nazwa pliku czy releasu sie zgadza)
	SUBTITLE_GOOD
};

// struktura opisujaca napisy
struct QNapiSubtitleInfo
{
	QNapiSubtitleInfo(QString l = "", QString e = "", QString u = "",
					  QString n = "", QString c = "", QString f = "",
					  QNapiSubtitleResolution r = SUBTITLE_UNKNOWN)
		: lang(l), engine(e), url(u), name(n), comment(c), format(f),
		  resolution(r) {}
	QString lang;
	QString engine;
	QString url;
	QString name;
	QString comment;
	QString format;
	QNapiSubtitleResolution resolution;
};

#endif

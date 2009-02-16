/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

#ifndef __MOVIEINFO__H__
#define __MOVIEINFO__H__

#include <QFile>
#include <cstring>
#include <cmath>

struct MainAVIHeader
{
	int dwMicroSecPerFrame;
	int dwMaxBytesPerSec;
	int dwReserved1;
	int dwFlags;
	int dwTotalFrames;
	int dwInitialFrames;
	int dwStreams;
	int dwSuggestedBufferSize;
	int dwWidth;
	int dwHeight;
	int dwReserved[4];
};

const char RIFF_HEADER[4] = {'R','I','F','F'};

class MovieInfo
{
	public:
		int width, height, time;
		float fps;
		bool isErr;
		MovieInfo(const QString & filename);
};

#endif

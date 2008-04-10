/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

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

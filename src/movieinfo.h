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

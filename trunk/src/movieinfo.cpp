#include "movieinfo.h"

MovieInfo::MovieInfo(const QString & filename)
{
	fps = width = height = time = 0;
	isErr = false;

	QFile file(filename);

	if(!file.open(QIODevice::ReadOnly))
	{
		isErr = true;
		return;
	}

	char buff[4];
	file.read(buff, 4);

	if(strncmp(buff, RIFF_HEADER, 4) != 0)
	{
		isErr = true;
		return;
	}

	MainAVIHeader hdr;
	
	if(!file.seek(0x20))
	{
		isErr = true;
		file.close();
		return;
	}

	if(!file.read((char*)&hdr, sizeof(MainAVIHeader)))
	{
		isErr = true;
		file.close();
		return;
	}

	file.close();

	width = hdr.dwWidth;
	height = hdr.dwHeight;
	fps = 1000000.0f/(float)hdr.dwMicroSecPerFrame;
	time = (int)ceil((float)hdr.dwTotalFrames / fps);
}

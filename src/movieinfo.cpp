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

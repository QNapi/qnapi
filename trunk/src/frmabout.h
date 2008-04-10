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

#ifndef __FRMABOUT__H__
#define __FRMABOUT__H__

#include <QDesktopWidget>

#include "ui_frmabout.h"
#include "version.h"
#include "qnapiconfig.h"

class frmAbout: public QDialog
{
Q_OBJECT
	public:
		frmAbout(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmAbout() {};
	private:
		Ui::frmAbout ui;
};

#endif

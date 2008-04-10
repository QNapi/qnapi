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

#ifndef __FRMOPTIONS__H__
#define __FRMOPTIONS__H__

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QTextCodec>
#include <QDesktopWidget>

#include "ui_frmoptions.h"
#include "qnapiconfig.h"

#include "frmcreateuser.h"

class frmOptions: public QDialog
{
Q_OBJECT
	public:
		frmOptions(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmOptions() {};

	public slots:
		void writeConfig();
		void readConfig();

	private slots:
		void le7zPathChanged();
		void select7zPath();
		void leTmpPathChanged();
		void selectTmpPath();
		void pbRegisterClicked();
		void changeEncodingClicked();
		void autoDetectEncodingClicked();
		void showAllEncodingsClicked();
		void useBrushedMetalClicked();

	private:
		Ui::frmOptions ui;
		void showAllEncodings();
};

#endif

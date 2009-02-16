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

#ifndef __FRMNAPIPROJEKTCONFIG__H__
#define __FRMNAPIPROJEKTCONFIG__H__

#include <QDialog>
#include "ui_frmnapiprojektconfig.h"

#include "qnapiconfig.h"
#include "qnapiapp.h"

class frmNapiProjektConfig : public QDialog
{
Q_OBJECT

	public:

		frmNapiProjektConfig(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmNapiProjektConfig() {}

	public slots:
	
		void accept();
		void pbRegisterClicked();

	private:
		Ui::frmNapiProjektConfig ui;
		
		void load();
		
};



#endif

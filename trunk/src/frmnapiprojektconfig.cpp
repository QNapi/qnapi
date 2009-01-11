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

#include "frmnapiprojektconfig.h"

frmNapiProjektConfig::frmNapiProjektConfig(QWidget *parent, Qt::WFlags f)
	: QDialog(parent, f)
{
	ui.setupUi(this);

	load();	
	
	connect(ui.pbRegister, SIGNAL(clicked()), this, SLOT(pbRegisterClicked()));
}

void frmNapiProjektConfig::accept()
{
	GlobalConfig().setNick("NapiProjekt", ui.leNick->text());
	GlobalConfig().setPass("NapiProjekt", ui.lePass->text());
	QDialog::accept();
}

void frmNapiProjektConfig::pbRegisterClicked()
{
	((QNapiApp*)qApp)->showCreateUser();
	load();
}

void frmNapiProjektConfig::load()
{
	ui.leNick->setText(GlobalConfig().nick("NapiProjekt"));
	ui.lePass->setText(GlobalConfig().pass("NapiProjekt"));
}

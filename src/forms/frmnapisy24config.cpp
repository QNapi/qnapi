/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "frmnapisy24config.h"
#include "../qnapi.h"

frmNapisy24Config::frmNapisy24Config(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    ui.setupUi(this);
    QNapi q;
    q.addEngines(q.enumerateEngines());
    setWindowIcon(q.engineByName("Napisy24")->engineIcon());

    load();

    connect(ui.pbRegister, SIGNAL(clicked()), this, SLOT(pbRegisterClicked()));

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

void frmNapisy24Config::accept()
{
    GlobalConfig().setNick("Napisy24", ui.leNick->text());
    GlobalConfig().setPass("Napisy24", ui.lePass->text());
    QDialog::accept();
}

void frmNapisy24Config::pbRegisterClicked()
{
    ((QNapiApp*)qApp)->showN24CreateUser();
    load();
}

void frmNapisy24Config::load()
{
    ui.leNick->setText(GlobalConfig().nick("Napisy24"));
    ui.lePass->setText(GlobalConfig().pass("Napisy24"));
}

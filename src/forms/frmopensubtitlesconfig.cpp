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

#include "frmopensubtitlesconfig.h"
#include "qnapi.h"
#include "qnapiconfig.h"
#include "qnapiapp.h"


frmOpenSubtitlesConfig::frmOpenSubtitlesConfig(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    ui.setupUi(this);
    QNapi q;
    q.addEngines(q.enumerateEngines());
    setWindowIcon(q.engineByName("OpenSubtitles")->engineIcon());

    load();

    connect(ui.pbRegister, SIGNAL(clicked()), this, SLOT(pbRegisterClicked()));

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

void frmOpenSubtitlesConfig::accept()
{
    GlobalConfig().setNick("OpenSubtitles", ui.leNick->text());
    GlobalConfig().setPass("OpenSubtitles", ui.lePass->text());
    QDialog::accept();
}

void frmOpenSubtitlesConfig::pbRegisterClicked()
{
    ((QNapiApp*)qApp)->showCreateAccount("OpenSubtitles");
}

void frmOpenSubtitlesConfig::load()
{
    ui.leNick->setText(GlobalConfig().nick("OpenSubtitles"));
    ui.lePass->setText(GlobalConfig().pass("OpenSubtitles"));
}

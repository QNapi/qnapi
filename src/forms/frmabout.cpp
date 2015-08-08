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

#include "frmabout.h"

frmAbout::frmAbout(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);
    ui.lbTitle->setText(QString("QNapi ") + QNAPI_DISPLAYABLE_VERSION);

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

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

#ifndef __FRMMAIN__H__
#define __FRMMAIN__H__

#include <QMainWindow>

#include "ui_frmmain.h"

class frmMain : public QMainWindow
{
    Q_OBJECT
public:
    frmMain();

signals:

public slots:

private:
    Ui::frmMain ui;
};

#endif // __FRMMAIN__H__

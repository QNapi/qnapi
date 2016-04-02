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

#ifndef __FRMNAPISY24CONFIG__H__
#define __FRMNAPISY24CONFIG__H__

#include <QDialog>
#include "ui_frmnapisy24config.h"

#include "qnapiconfig.h"
#include "qnapiapp.h"

class frmNapisy24Config : public QDialog
{
Q_OBJECT

    public:

        frmNapisy24Config(QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~frmNapisy24Config() {}

    public slots:
    
        void accept();
        void pbRegisterClicked();

    private:
        Ui::frmNapisy24Config ui;
        
        void load();
        
};



#endif

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

#ifndef __FRMOpenSubtitlesCONFIG__H__
#define __FRMOpenSubtitlesCONFIG__H__

#include <QDialog>
#include "ui_frmopensubtitlesconfig.h"

#include "qnapiconfig.h"
#include "qnapiapp.h"

class frmOpenSubtitlesConfig : public QDialog
{
Q_OBJECT

    public:

        frmOpenSubtitlesConfig(QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~frmOpenSubtitlesConfig() {}

    public slots:
    
        void accept();
        void pbRegisterClicked();

    private:
        Ui::frmOpenSubtitlesConfig ui;
        
        void load();
        
};



#endif

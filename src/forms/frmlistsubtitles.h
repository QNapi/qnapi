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


#ifndef __FRMLISTSUBTITLES__H__
#define __FRMLISTSUBTITLES__H__


#include <QDialog>

#include "ui_frmlistsubtitles.h"
#include "qnapisubtitleinfo.h"

class frmListSubtitles : public QDialog
{
Q_OBJECT

    public:

        frmListSubtitles(QWidget *parent = 0, Qt::WindowFlags f = 0);
        ~frmListSubtitles() {}

    public slots:

        void setFileName(const QString & name);
        void setSubtitlesList(const QList<QNapiSubtitleInfo> & list);
        int getSelectedIndex();
        void accept();

    private:
        Ui::frmListSubtitles ui;
        
};

#endif

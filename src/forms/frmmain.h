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
#include <QDragEnterEvent>
#include <QDropEvent>

#include "ui_frmmain.h"

class frmMain : public QMainWindow
{
    Q_OBJECT
public:
    frmMain();

signals:

    void droppedFiles(const QStringList &);
    void droppedDirectory(const QString &);

    void settings();
    void about();
    void help();
    void quit();

    void download();
    void scan();
    void convert();

    void engineDownload(QString);
    void engineCreateAccount(QString);

public slots:

private:

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void closeEvent (QCloseEvent *event);

    bool acceptDraggedUrls(const QList<QUrl> & urls) const;
    bool hasOnlyMovieFiles(const QList<QUrl> & urls) const;
    bool hasOnlyDirectories(const QList<QUrl> & urls) const;

    Ui::frmMain ui;
};

#endif // __FRMMAIN__H__

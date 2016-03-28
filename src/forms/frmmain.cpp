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

#include "frmmain.h"

#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QFileInfo>
#include <QDesktopWidget>

#include "qnapiconfig.h"

frmMain::frmMain()
{
    ui.setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(Qt::WindowTitleHint
                 | Qt::WindowMinimizeButtonHint
                 | Qt::WindowSystemMenuHint
                 | Qt::WindowCloseButtonHint);

    statusBar()->hide();

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());

    connect(ui.actionSettings, SIGNAL(triggered()), this, SIGNAL(settings()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SIGNAL(about()));
    connect(ui.actionHelp, SIGNAL(triggered()), this, SIGNAL(help()));
    connect(ui.actionQuit, SIGNAL(triggered()), this, SIGNAL(quit()));

    connect(ui.actionSubtitleDownload, SIGNAL(triggered()), this, SIGNAL(download()));
    connect(ui.actionDirectoryScan, SIGNAL(triggered()), this, SIGNAL(scan()));
    connect(ui.actionConvert, SIGNAL(triggered()), this, SIGNAL(convert()));

    connect(ui.actionNPDownload, SIGNAL(triggered()), this, SIGNAL(downloadNP()));
    connect(ui.actionNPConfigure, SIGNAL(triggered()), this, SIGNAL(configureNP()));
    connect(ui.actionNPRegister, SIGNAL(triggered()), this, SIGNAL(registerNP()));

    connect(ui.actionOSDownload, SIGNAL(triggered()), this, SIGNAL(downloadOS()));
    connect(ui.actionOSUpload, SIGNAL(triggered()), this, SIGNAL(uploadOS()));
    connect(ui.actionOSConfigure, SIGNAL(triggered()), this, SIGNAL(configureOS()));
    connect(ui.actionOSRegister, SIGNAL(triggered()), this, SIGNAL(registerOS()));

    connect(ui.actionN24Download, SIGNAL(triggered()), this, SIGNAL(downloadN24()));
    //connect(ui.actionN24Configure, SIGNAL(triggered()), this, SIGNAL(configureN24()));
    connect(ui.actionN24Register, SIGNAL(triggered()), this, SIGNAL(registerN24()));
}

void frmMain::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && acceptDraggedUrls(event->mimeData()->urls()))
        event->acceptProposedAction();
}

void frmMain::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    const QList<QUrl> & urls = event->mimeData()->urls();

    if(hasOnlyMovieFiles(urls)) {
        QStringList paths;
        foreach(QUrl url, urls) {
            paths << url.toLocalFile();
        }
        emit droppedFiles(paths);
    } else if(hasOnlyDirectories(urls)) {
        emit droppedDirectory(urls.first().toLocalFile());
    }
}

void frmMain::closeEvent(QCloseEvent *event)
{
    ui.actionQuit->trigger();
    QMainWindow::closeEvent(event);
}

bool frmMain::acceptDraggedUrls(const QList<QUrl> & urls) const
{
    return hasOnlyMovieFiles(urls) || hasOnlyDirectories(urls);
}

bool frmMain::hasOnlyMovieFiles(const QList<QUrl> & urls) const
{
    QSet<QString> exts = GlobalConfig().movieExtensions().toSet();

    foreach(QUrl url, urls) {
        if(!url.isLocalFile()) {
            return false;
        }
        QFileInfo fi(url.toLocalFile());
        if(!fi.isFile() || !exts.contains(fi.suffix()))
            return false;
    }
    return true;
}


bool frmMain::hasOnlyDirectories(const QList<QUrl> & urls) const
{
    foreach(QUrl url, urls) {
        if(!url.isLocalFile() || !QFileInfo(url.toLocalFile()).isDir())
            return false;
    }
    return true;
}

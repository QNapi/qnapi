/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "qnapiopendialog.h"
#include "libqnapi.h"

QNapiOpenDialog::QNapiOpenDialog(QWidget* parent, const QString& caption,
                                 const QString& init_path,
                                 FilterMode filterMode)
    : QFileDialog(parent), staticConfig(LibQNapi::staticConfig()) {
  setAttribute(Qt::WA_QuitOnClose, false);
  setWindowTitle(caption);

  if (filterMode == Movies) {
    setNameFilter(tr("Video files (%1);;All files (*.*)")
                      .arg(staticConfig->movieExtensionsFilter()));
  } else if (filterMode == Subtitles) {
    setNameFilter(tr("Subtitles files (%1);;All files (*.*)")
                      .arg(staticConfig->subtitleExtensionsFilter()));
  }

  if (QFileInfo(init_path).isDir())
    setDirectory(init_path);
  else
    setDirectory(QDir::currentPath());

  QStringList sideUrls;

#ifdef Q_OS_MAC
  sideUrls << "/Volumes";
#endif

  sideUrls << QString(QDir::homePath() + "/Movies")
           << QString(QDir::homePath() + "/movies")
           << QString(QDir::homePath() + "/Video")
           << QString(QDir::homePath() + "/video")
           << QString(QDir::homePath() + "/Videos")
           << QString(QDir::homePath() + "/videos")
           << QString(QDir::homePath() + "/Filmy")
           << QString(QDir::homePath() + "/wideo");

  QList<QUrl> urls = sidebarUrls();

  foreach (QString sideUrl, sideUrls) {
    if (!QDir().exists(sideUrl)) continue;
    QUrl url = QUrl::fromLocalFile(sideUrl);
    if (!urls.contains(url)) urls << url;
  }

  setSidebarUrls(urls);
}

bool QNapiOpenDialog::selectFile() {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  files.clear();
  QString file = getOpenFileName(this, windowTitle(), directory().path(),
                                 nameFilters().join("\n"));

  if (!file.isEmpty()) files << file;

  return !file.isEmpty();
#else
  if (!placeWindow()) return false;
  setFileMode(QFileDialog::ExistingFile);
  return exec();
#endif
}

bool QNapiOpenDialog::selectFiles() {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  files = getOpenFileNames(this, windowTitle(), directory().path(),
                           nameFilters().join("\n"));
  return !files.isEmpty();
#else
  if (!placeWindow()) return false;
  setFileMode(QFileDialog::ExistingFiles);
  return exec();
#endif
}

bool QNapiOpenDialog::selectDirectory() {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  files.clear();
  QString dir = getExistingDirectory(this, windowTitle(), directory().path());

  if (dir == directory().path()) dir = "";

  if (!dir.isEmpty()) files << dir;

  return !dir.isEmpty();
#else
  if (!placeWindow()) return false;
  // QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks ?
  setFileMode(QFileDialog::DirectoryOnly);
  return exec();
#endif
}

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
QStringList QNapiOpenDialog::selectedFiles() const { return files; }
#endif

bool QNapiOpenDialog::placeWindow() {
  if (isVisible()) {
    raise();
    return false;
  }

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());

  return true;
}

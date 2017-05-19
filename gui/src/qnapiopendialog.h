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

#ifndef __QNAPIOPENDIALOG__H__
#define __QNAPIOPENDIALOG__H__

#include "config/staticconfig.h"

#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>

class QNapiOpenDialog : public QFileDialog {
  Q_OBJECT
 public:
  enum FilterMode { None, Movies, Subtitles };

  QNapiOpenDialog(QWidget* parent = 0, const QString& caption = QString(),
                  const QString& init_path = QString(),
                  FilterMode filterMode = None);

  bool selectFile();
  bool selectFiles();
  bool selectDirectory();

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  QStringList selectedFiles() const;
#endif

 private:
  bool placeWindow();

  QSharedPointer<const StaticConfig> staticConfig;

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  QStringList files;
#endif
};

#endif

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

#ifndef __FRMOPENSUBTITLESCONFIG__H__
#define __FRMOPENSUBTITLESCONFIG__H__

#include <QDialog>
#include "config/engineconfig.h"

#include "ui_frmopensubtitlesconfig.h"

class frmOpenSubtitlesConfig : public QDialog {
  Q_OBJECT

 public:
  frmOpenSubtitlesConfig(const EngineConfig &config, QWidget *parent = 0,
                         Qt::WindowFlags f = 0);
  ~frmOpenSubtitlesConfig() {}

  EngineConfig getConfig() const;

 public slots:

  void accept();
  void pbRegisterClicked();

 private:
  Ui::frmOpenSubtitlesConfig ui;
  EngineConfig config;
};

#endif

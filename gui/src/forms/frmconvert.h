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

#ifndef __FRMCONVERT_H__
#define __FRMCONVERT_H__

#include <Maybe.h>
#include <QDialog>
#include <QSharedPointer>
#include "config/postprocessingconfig.h"
#include "config/staticconfig.h"
#include "subconvert/subtitleconverter.h"
#include "subconvert/subtitleformatsregistry.h"
#include "ui_frmconvert.h"

class frmConvert : public QDialog {
  Q_OBJECT
 public:
  frmConvert(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~frmConvert() {}

 private:
  Ui::frmConvert ui;
  QSharedPointer<const StaticConfig> staticConfig;
  const PostProcessingConfig ppConfig;
  QSharedPointer<const SubtitleFormatsRegistry> subtitleFormatsRegistry;
  SubtitleConverter subConverter;
  bool targetFileNameSelected;
  QString srcFormat, targetFormat;

  void checkFPSNeeded();
  Maybe<QString> determineMovieFPS(const QString &defaultMovieFilePath);
  void generateTargetFileName();

 private slots:

  void srcSubSelectClicked();
  void srcSubFileLoaded(const QString &srcSubFileName);
  void targetFormatChanged(int targetFormatIdx);
  void movieFPSSelectClicked();
  void targetMovieFPSSelectClicked();
  void targetExtensionChanged();
  void subDelayToggled();
  void convertClicked();
};

#endif  // __FRMCONVERT_H__

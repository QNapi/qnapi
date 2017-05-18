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

#ifndef __FRMSUMMARY__H__
#define __FRMSUMMARY__H__

#include "engines/subtitledownloadenginesregistry.h"

#include "subtitleinfo.h"
#include "ui_frmsummary.h"

#include <QList>
#include <QSharedPointer>
#include <QWidget>

class frmSummary : public QDialog {
  Q_OBJECT
 public:
  frmSummary(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~frmSummary() {}

  void setSummaryList(QList<SubtitleInfo> list);

 private:
  Ui::frmSummary ui;

  QSharedPointer<const SubtitleDownloadEnginesRegistry> enginesRegistry;
};

#endif

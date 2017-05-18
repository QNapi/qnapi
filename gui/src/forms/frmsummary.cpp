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

#include "frmsummary.h"
#include "libqnapi.h"

#include "subdatawidget.h"

#include <QDesktopWidget>
#include <QListWidget>

frmSummary::frmSummary(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
  ui.setupUi(this);

  setAttribute(Qt::WA_QuitOnClose, false);

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
}

void frmSummary::setSummaryList(QList<SubtitleInfo> list) {
  std::sort(list.begin(), list.end());

  QIcon succIcon(":/ui/accept.png"), failIcon(":/ui/exclamation.png");

  ui.lwSummary->clear();
  ui.lwSummary->setFocusPolicy(Qt::NoFocus);

  int i = 0, goodCount = 0, badCount = 0;
  foreach (SubtitleInfo s, list) {
    bool isGood = s.resolution != SUBTITLE_NONE;

    QListWidgetItem *listItem = new QListWidgetItem();

    ui.lwSummary->addItem(listItem);

    subDataWidget *subData = new subDataWidget();

    if (isGood) {
      ++goodCount;
      QString lang_path = QString(":/languages/") + s.lang + ".png";
      QIcon engineIcon =
          QIcon(QPixmap(enginesRegistry->enginePixmapData(s.engine)));
      subData->setSubData(succIcon, s.name, QIcon(lang_path), engineIcon);
    } else {
      ++badCount;
      subData->setSubData(failIcon, s.name);
    }

    ui.lwSummary->setItemWidget(listItem, subData);
    listItem->setSizeHint(subData->sizeHint());

    ++i;
  }

  ui.lwSummary->setMinimumWidth(ui.lwSummary->sizeHintForColumn(0) + 20);
  this->adjustSize();

  ui.lbSuccess->setVisible(goodCount != 0);
  ui.lbFail->setVisible(badCount != 0);

  ui.lbSuccess->setText(tr("Subtitles downloaded for %1 %2")
                            .arg(goodCount)
                            .arg(goodCount > 1 ? tr("files") : tr("file")));
  ui.lbFail->setText(tr("Could not download subtitles for %1 %2")
                         .arg(badCount)
                         .arg(badCount > 1 ? tr("files") : tr("file")));
}

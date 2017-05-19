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

#include "libqnapi.h"

#include "frmlistsubtitles.h"
#include "subdatawidget.h"

#include <QDesktopWidget>
#include <QMessageBox>
#include <QtWidgets/QHeaderView>

frmListSubtitles::frmListSubtitles(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      enginesRegistry(LibQNapi::subtitleDownloadEngineRegistry()) {
  ui.setupUi(this);

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
}

void frmListSubtitles::setFileName(const QString &name) {
  ui.lbSelectSubtitles->setText(tr("Select the subtitles which you want to "
                                   "download for video file<br><br><b>%1</b>")
                                    .arg(name));
}

void frmListSubtitles::setSubtitlesList(QList<SubtitleInfo> list) {
  ui.twSubtitles->clear();

  int i = 0, goodCount = 0, badCount = 0;
  foreach (SubtitleInfo s, list) {
    bool highlight = (s.resolution != SUBTITLE_UNKNOWN);

    QBrush brush((s.resolution == SUBTITLE_GOOD) ? QColor(qRgb(200, 255, 200))
                                                 : QColor(qRgb(255, 200, 200)));

    QListWidgetItem *listItem = new QListWidgetItem();

    ui.twSubtitles->addItem(listItem);

    subDataWidget *subData = new subDataWidget();

    if (highlight) listItem->setBackground(brush);

    QString lang_path = QString(":/languages/") + s.lang + ".png";

    if (highlight && (s.resolution == SUBTITLE_GOOD)) {
      ++goodCount;
    } else if (highlight && (s.resolution == SUBTITLE_BAD)) {
      ++badCount;
    }

    subData->setSubData(
        s.name, s.format, QIcon(lang_path),
        QIcon(QPixmap(enginesRegistry->enginePixmapData(s.engine))));

    ui.twSubtitles->setItemWidget(listItem, subData);
    listItem->setSizeHint(subData->sizeHint());

    ++i;
  }

  if (goodCount == 0) ui.lbGreenHint->hide();
  if (badCount == 0) ui.lbRedHint->hide();
}

int frmListSubtitles::getSelectedIndex() {
  QModelIndexList selected =
      ui.twSubtitles->selectionModel()->selectedIndexes();
  return selected.first().row();
}

void frmListSubtitles::accept() {
  if (ui.twSubtitles->selectedItems().size() == 0) {
    QMessageBox::warning(this, tr("No subtitle selected"),
                         tr("You need to select a subtitle from the list!"));
  } else {
    QDialog::accept();
  }
}

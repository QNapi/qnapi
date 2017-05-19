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

#ifndef __QCHECKEDLISTWIDGET__H__
#define __QCHECKEDLISTWIDGET__H__

#include <QtWidgets>

class QCheckedListWidget : public QListWidget {
 public:
  QCheckedListWidget(QWidget* parent = 0) : QListWidget(parent) {
    setMouseTracking(true);
  }

  void setItemCheckState(int i, Qt::CheckState state) {
    QListWidgetItem* it = item(i);
    if (it) it->setCheckState(state);
  }

  Qt::CheckState itemCheckState(int i) {
    QListWidgetItem* it = item(i);
    return it ? it->checkState() : Qt::Unchecked;
  }

  void selectAll() {
    for (int i = 0; i < count(); ++i) {
      item(i)->setCheckState(Qt::Checked);
    }
  }

  void unselectAll() {
    for (int i = 0; i < count(); ++i) {
      item(i)->setCheckState(Qt::Unchecked);
    }
  }

  void invertSelection() {
    Qt::CheckState state;
    for (int i = 0; i < count(); ++i) {
      state = item(i)->checkState();
      state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
      item(i)->setCheckState(state);
    }
  }
};

#endif  // __QCHECKEDLISTWIDGET__H__

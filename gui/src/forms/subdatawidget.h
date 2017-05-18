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

#ifndef SUBDATAWIDGET_H
#define SUBDATAWIDGET_H

#include <QWidget>
#include "ui_subDataWidget.h"

class subDataWidget : public QWidget {
  Q_OBJECT

 public:
  explicit subDataWidget(QWidget *parent = 0);
  void setSubData(const QIcon &stateIcon, QString name, const QIcon &langIcon,
                  const QIcon &engineIcon);
  void setSubData(const QIcon &stateIcon, QString name);
  void setSubData(QString name, QString ext, const QIcon &langIcon,
                  const QIcon &engineIcon);
  void clearAll();
  ~subDataWidget();

 private:
  Ui::subDataWidget *ui;
};

#endif  // SUBDATAWIDGET_H

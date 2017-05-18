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

#include "subdatawidget.h"

subDataWidget::subDataWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::subDataWidget) {
  ui->setupUi(this);
}

subDataWidget::~subDataWidget() { delete ui; }

void subDataWidget::clearAll() {
  ui->place1->clear();
  ui->place2->clear();
  ui->place3->clear();
  ui->place4->clear();
}

void subDataWidget::setSubData(const QIcon &stateIcon, QString name,
                               const QIcon &langIcon, const QIcon &engineIcon) {
  ui->place1->setPixmap(stateIcon.pixmap(16));
  ui->place2->setText(name);
  ui->place3->setPixmap(langIcon.pixmap(16));
  ui->place4->setPixmap(engineIcon.pixmap(16));
}

void subDataWidget::setSubData(const QIcon &stateIcon, QString name) {
  this->clearAll();

  ui->place3->hide();
  ui->place4->hide();

  ui->place1->setPixmap(stateIcon.pixmap(16));
  ui->place2->setText(name);
}

void subDataWidget::setSubData(QString name, QString ext, const QIcon &langIcon,
                               const QIcon &engineIcon) {
  this->clearAll();

  ui->place1->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::Preferred);
  ui->place2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  ui->place1->setText(name);
  ui->place2->setText(ext);
  ui->place3->setPixmap(langIcon.pixmap(16));
  ui->place4->setPixmap(engineIcon.pixmap(16));
}

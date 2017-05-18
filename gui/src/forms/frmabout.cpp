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

#include "frmabout.h"
#include "libqnapi.h"

frmAbout::frmAbout(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f) {
  ui.setupUi(this);

  setAttribute(Qt::WA_QuitOnClose, false);
  ui.lbTitle->setText(QString("QNapi ") + LibQNapi::displayableVersion());
  ui.lbQtVersion->setText(QString("Qt version: ") + qVersion());

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());

  ui.lbQNapiIcon->setPixmap(QIcon(":/icon/qnapi.png").pixmap(64));
  ui.lbNPBanner->setPixmap(QIcon(":/icon/napiprojekt.jpg").pixmap(128, 35));
  ui.lbOSBanner->setPixmap(QIcon(":/icon/opensubtitles.png").pixmap(150, 51));
  ui.lbN24Banner->setPixmap(QIcon(":/icon/napisy24.gif").pixmap(130, 34));
}

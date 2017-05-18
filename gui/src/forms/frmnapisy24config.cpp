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

#include <QDesktopServices>
#include <QDesktopWidget>

#include "engines/napisy24downloadengine.h"
#include "frmnapisy24config.h"

frmNapisy24Config::frmNapisy24Config(const EngineConfig &config,
                                     QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), config(config) {
  ui.setupUi(this);

  ui.leNick->setText(config.nick());
  ui.lePass->setText(config.password());

  QIcon napisy24Icon = QIcon(QPixmap(Napisy24DownloadEngine::pixmapData));
  setWindowIcon(napisy24Icon);

  connect(ui.pbRegister, SIGNAL(clicked()), this, SLOT(pbRegisterClicked()));

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());
}

EngineConfig frmNapisy24Config::getConfig() const { return config; }

void frmNapisy24Config::accept() {
  config = config.setNick(ui.leNick->text()).setPassword(ui.lePass->text());
  QDialog::accept();
}

void frmNapisy24Config::pbRegisterClicked() {
  Maybe<QUrl> maybeRegistrationUrl =
      Napisy24DownloadEngine::metadata.registrationUrl();

  if (maybeRegistrationUrl) {
    QDesktopServices::openUrl(maybeRegistrationUrl.value());
  }
}

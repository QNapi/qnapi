/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

frmSummary::frmSummary(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);

    // workaround dla compiza?
    move((QApplication::desktop()->width() - width()) / 2, 
        (QApplication::desktop()->height() - height()) / 2);
}

void frmSummary::setSummaryList(const QStringList & listSuccess, const QStringList & listFailures)
{
    ui.lwSummary->clear();

    ui.lbSuccess->setVisible(!listSuccess.isEmpty());
    ui.lbFail->setVisible(!listFailures.isEmpty());

    QIcon succIcon(":/ui/accept.png"), failIcon(":/ui/exclamation.png");

    foreach(QString successItem, listSuccess)
    {
        ui.lwSummary->addItem(new QListWidgetItem(succIcon, QFileInfo(successItem).fileName()));
    }

    foreach(QString failureItem, listFailures)
    {
        ui.lwSummary->addItem(new QListWidgetItem(failIcon, QFileInfo(failureItem).fileName()));
    }

    ui.lbSuccess->setText(tr("Pobrano napisy dla %1 %2")
                          .arg(listSuccess.size())
                          .arg(tr(listSuccess.size() > 1 ? "plików" : "pliku")));

    ui.lbFail->setText(tr("Nie pobrano napisów dla %1 %2")
                       .arg(listFailures.size())
                       .arg(tr(listFailures.size() > 1 ? "plików" : "pliku")));
}

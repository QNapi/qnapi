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

void frmSummary::setSuccessList(const QStringList & list)
{
	if(list.isEmpty())
	{
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabSuccess));
		return;
	}

	ui.lwSuccess->clear();
	foreach(QString item, list)
	{
		ui.lwSuccess->addItem(new QListWidgetItem(QIcon(":/ui/accept.png"),
												QFileInfo(item).fileName()));
	}
	ui.lwSuccess->sortItems();
	ui.tabWidget->setTabText(ui.tabWidget->indexOf(ui.tabSuccess),
								tr("Pobrano napisy dla %1 %2")
								.arg(list.size())
								.arg(tr(list.size() > 1 ? "plików" : "pliku")));
}

void frmSummary::setFailedList(const QStringList & list)
{
	if(list.isEmpty())
	{
		ui.tabWidget->removeTab(ui.tabWidget->indexOf(ui.tabFail));
		return;
	}

	ui.lwFail->clear();
	foreach(QString item, list)
	{
		ui.lwFail->addItem(new QListWidgetItem(QIcon(":/ui/exclamation.png"),
												QFileInfo(item).fileName()));
	}
	ui.lwFail->sortItems();

	ui.tabWidget->setTabText(ui.tabWidget->indexOf(ui.tabFail),
								tr("Nie pobrano napisów dla %1 %2")
								.arg(list.size())
								.arg(tr(list.size() > 1 ? "plików" : "pliku")));
}

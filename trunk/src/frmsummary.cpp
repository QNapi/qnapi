/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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

frmSummary::frmSummary(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif
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
		ui.lwSuccess->addItem(new QListWidgetItem(QIcon(":/ui/icon_accept.png"),
												QFileInfo(item).fileName()));
	}
	ui.lwSuccess->sortItems();
	ui.tabWidget->setTabText(0, tr("Dopasowano napisy dla %1 %2")
								.arg(list.size()).arg(tr(list.size() > 1 ? "plików" : "pliku")));
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
		ui.lwFail->addItem(new QListWidgetItem(QIcon(":/ui/icon_cancel.png"),
												QFileInfo(item).fileName()));
	}
	ui.lwFail->sortItems();

	ui.tabWidget->setTabText(1, tr("Nie udało się dopasować napisów dla %1 %2")
								.arg(list.size()).arg(tr(list.size() > 1 ? "plików" : "pliku")));
}

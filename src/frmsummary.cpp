/*************************************************************************

    QNapi
    Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

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

void frmSummary::setFileList(const QStringList & list)
{
	ui.lwFiles->clear();
	foreach(QString item, list)
	{
		ui.lwFiles->addItem(new QListWidgetItem(QIcon(":/ui/icon_accept.png"),
												QFileInfo(item).fileName()));
	}
	ui.lwFiles->sortItems();
}

void frmSummary::setFailedCount(unsigned int failed)
{
	if(failed == 0)
	{
		ui.lbFailed->hide();
	}
	else
	{
		ui.lbFailed->setText(tr("Nie udało się dopasować napisów dla %1 %2!").arg(failed)
								.arg(tr((failed > 1) ? "plików" : "pliku")));
	}
}

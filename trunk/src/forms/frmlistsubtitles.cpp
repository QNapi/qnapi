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


#include "frmlistsubtitles.h"

frmListSubtitles::frmListSubtitles(QWidget *parent, Qt::WFlags f)
	: QDialog(parent, f)
{
	ui.setupUi(this);
}

void frmListSubtitles::setFileName(const QString & name)
{
	ui.lbSelectSubtitles->setText(QString(	"Z poniższej listy wybierz napisy, które"
											" chcesz dopasować do pliku <b>%1</b>:")
										.arg(name));
}

void frmListSubtitles::setSubtitlesList(const QList<QNapiSubtitleInfo> & list)
{
	QNapi n;
	n.addEngines(n.enumerateEngines());	

	ui.twSubtitles->clear();
	ui.twSubtitles->setColumnCount(4);
	ui.twSubtitles->setRowCount(list.size());

	ui.twSubtitles->horizontalHeader()->hide();
	ui.twSubtitles->verticalHeader()->hide();
	ui.twSubtitles->verticalHeader()->setDefaultSectionSize(20);
	ui.twSubtitles->verticalHeader()->setResizeMode(QHeaderView::Fixed);

	int i = 0;
	foreach(QNapiSubtitleInfo s, list)
	{
		QTableWidgetItem *item;

		bool highlight = (s.resolution != SUBTITLE_UNKNOWN);

		QBrush brush((s.resolution == SUBTITLE_GOOD) ? QColor(qRgb(200, 255, 200)) : QColor(qRgb(255, 200, 200)));		

		QString lang_path = QString(":/languages/") + s.lang + ".gif";
		if(QFile::exists(lang_path))
		{
			item = new QTableWidgetItem(QIcon(lang_path), "");
		}
		else
		{
			item = new QTableWidgetItem(s.lang);
		}

		if(highlight) item->setBackground(brush);
		item->setToolTip(s.comment);
		ui.twSubtitles->setItem(i, 1, item);

		item = new QTableWidgetItem(s.name);
		if(highlight) item->setBackground(brush);
		item->setToolTip(s.comment);
		ui.twSubtitles->setItem(i, 2, item);

		item = new QTableWidgetItem(s.format);
		if(highlight) item->setBackground(brush);
		item->setToolTip(s.comment);
		ui.twSubtitles->setItem(i, 3, item);

		QNapiAbstractEngine *e = n.engineByName(s.engine);
		if(e)
		{
			item = new QTableWidgetItem(e->engineIcon(), "");
			if(highlight) item->setBackground(brush);
			item->setToolTip(s.comment);
			ui.twSubtitles->setItem(i, 0, item);
		}

		++i;
	}

	ui.twSubtitles->resizeColumnsToContents();
}

int frmListSubtitles::getSelectedIndex()
{
	return ui.twSubtitles->currentRow();
}

void frmListSubtitles::accept()
{
	if(ui.twSubtitles->selectedItems().size() == 0)
	{
		QMessageBox::warning(	this,
								"Nie wybrano napisów z listy",
								"Musisz wybrać napisy z listy!");
	}
	else
	{
		QDialog::accept();
	}
}



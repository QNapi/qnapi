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

#ifndef __FRMOPTIONS__H__
#define __FRMOPTIONS__H__

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QTextCodec>
#include <QDesktopWidget>

#include "ui_frmoptions.h"
#include "qnapiconfig.h"

#include "frmcreateuser.h"
#include "qnapilanguage.h"


class frmOptions: public QDialog
{
Q_OBJECT
	public:
		frmOptions(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmOptions();

	public slots:
		void writeConfig();
		void readConfig();

	private slots:
		void le7zPathChanged();
		void select7zPath();
		void leTmpPathChanged();
		void selectTmpPath();

		void twEnginesSelectionChanged();
		void twEnginesItemChanged(QTableWidgetItem * item);
		void pbMoveUpClicked();
		void pbMoveDownClicked();
		void pbEngineConfClicked();
		void pbEngineInfoClicked();	

		void changeEncodingClicked();
		void autoDetectEncodingClicked();
		void showAllEncodingsClicked();
		void useBrushedMetalClicked();

		void restoreDefaults();

	private:
		Ui::frmOptions ui;
		void showAllEncodings();
		
};

#endif

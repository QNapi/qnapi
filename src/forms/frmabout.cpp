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

#include "frmabout.h"

frmAbout::frmAbout(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif

	setAttribute(Qt::WA_QuitOnClose, false);
	ui.lbTitle->setText(QString("QNapi ") + QNAPI_DISPLAYABLE_VERSION);

const QString www = tr("<a href=\"http://krzemin.iglu.cz/qnapi\">http://krzemin.iglu.cz/qnapi</a>");
const QString author = tr("Copyright (C) 2008-2009 Krzemin &lt;<a href=\"mailto:pkrzemin@o2.pl\">pkrzemin@o2.pl</a>&gt;");
const QString useos = tr("Program korzysta z bazy <a href=\"http://www.opensubtitles.org\">http://www.opensubtitles.org</a>");
const QString usenp = tr("Program korzysta z bazy <a href=\"http://www.napiprojekt.pl\">http://www.napiprojekt.pl</a>");

	ui.lbWWW->setText(www);
	ui.lbAuthor->setText(author);
	ui.lbUseOS->setText(useos);
	ui.lbUseNP->setText(usenp);

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

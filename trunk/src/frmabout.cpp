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

#include "frmabout.h"

frmAbout::frmAbout(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#endif

	setAttribute(Qt::WA_QuitOnClose, false);
	ui.lbTitle->setText(QString("QNapi ") + QNAPI_VERSION);

const QString aboutText = tr(
"<html><body><p>QNapi jest nieoficjalnym, wolnym klonem programu NAPI-PROJEKT, "
"napisanym z myślą o użytkownikach systemów Linux, Mac OS X oraz innych, pod które "
"oryginalny klient NAPI nie jest dostępny.</p><p></p><p><b>Ten program rozprowadzany "
"jest na licencji GNU General Public License w wersji 2. lub każdej nowszej.</b></p>"
"<p></p><p><b>Autor</b>: Krzemin &lt;<a href=\"mailto:pkrzemin@o2.pl?subject=QNapi\">"
"pkrzemin@o2.pl</a>&gt;</p><p><b>Prawa autorskie:</b> Copyright (C) 2008 Krzemin</p>"
"<p><b>WWW</b>:  <a href=\"http://krzemin.iglu.cz/qnapi\">http://krzemin.iglu.cz/qnapi</a></p>"
"</body></html>");
const QString thanksText = tr(
"<html><body><p>Podziękowania należą się wszystkim osobom, które aktywnie wsparły rozwój "
"QNapi, a w szczególności:<ul><li>adrian5632 &lt;<a href=\"mailto:adrian5632@gmail.com\">"
"adrian5632@gmail.com</a>&gt; - opiekun wersji na Mac OS X, liczne patche i poprawki</li>"
"<li>Rafał Wójcik &lt;<a href=\"mailto:rafal.wojcik@gmail.com\">rafal.wojcik@gmail.com</a>&gt;"
" - port dla Windows wraz z instalatorem</li></ul></p><p>A także paczkującym dla różnych "
"dystrybucji:</p><p>obi_gl, Jakub Schmidtke, Jan Stępień, Łukasz Jagiełło</p><p>"
"...oraz wszystkim tym, o których zapomniałem ;)</p></body></html>");
const QString supportText = tr(
"<html><body><p>QNapi zaoszczędził Ci sporo pracy wysiłku przy szukaniu napisów do filmów? "
"Uwaszasz, że to dobry program i powinien się dalej rozwijać? Chciałbyś/aś wesprzeć rozwój programu?</p>"
"<p>Oprócz promowania go wśród znajomych, na forach internetowych, blogach itp. możesz również "
"rozważyć wsparcie autora drobną sumką, przelewając ją na konto:</p>"
"<p>73 1020 3974 0000 5202 0098 8774</p>"
"<p>Będę wdzięczny nawet za najmniejsze wpłaty ;)</p>"
);

	ui.tbAbout->setText(aboutText);
	ui.tbThanks->setText(thanksText);
	ui.tbSupport->setText(supportText);

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

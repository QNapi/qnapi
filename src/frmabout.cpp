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
"jest na licencji GNU General Public License.</b></p><p></p><p><b>Autor</b>: "
"Krzemin &lt;<a href=\"mailto:pkrzemin@o2.pl?subject=QNapi\">pkrzemin@o2.pl</a>&gt;"
"</p><p><b>WWW</b>: <a href=\"http://krzemin.iglu.cz/qnapi\">http://krzemin.iglu.cz/qnapi</a>"
"</p></body></html>");
const QString thanksText = tr(
"<html><body><p>Podziękowania należą się wszystkim osobom, które aktywnie wsparły rozwój "
"QNapi, a w szczególności:<ul><li>adrian5632 &lt;<a href=\"mailto:adrian5632@gmail.com\">"
"adrian5632@gmail.com</a>&gt; - opiekun wersji na Mac OS X, liczne patche i poprawki</li>"
"<li>Rafał Wójcik &lt;<a href=\"mailto:rafal.wojcik@gmail.com\">rafal.wojcik@gmail.com</a>&gt;"
" - port dla Windows wraz z instalatorem</li></ul></p><p>A także paczkującym dla różnych "
"dystrybucji:</p><p>obi_gl, Jakub Schmidtke, Jan Stępień, Łukasz Jagiełło</p><p>"
"...oraz wszystkim tym, o których zapomniałem ;)</p></body></html>");

	ui.tbAbout->setText(aboutText);
	ui.tbThanks->setText(thanksText);

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

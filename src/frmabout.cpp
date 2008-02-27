/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmabout.h"

frmAbout::frmAbout(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
#ifdef Q_WS_MAC
	setAttribute(Qt::WA_MacBrushedMetal, true);
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

	ui.tbAbout->setText(aboutText);

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);
}

void frmAbout::resizeEvent(QResizeEvent *resize)
{
	int b = 2;
	ui.horizontalLayout->setGeometry(QRect(b, b, resize->size().width() - b*2,
										resize->size().height() - b*2));
}

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
}

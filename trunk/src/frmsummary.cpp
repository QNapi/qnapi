/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

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

/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __FRMSUMMARY__H__
#define __FRMSUMMARY__H__

#include <QDesktopWidget>
#include <QListWidget>
#include <QFileInfo>

#include "qnapiconfig.h"

#include "ui_frmsummary.h"

class frmSummary: public QDialog
{
Q_OBJECT
	public:
		frmSummary(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmSummary() {};

		void setFileList(const QStringList & list);
		void setFailedCount(unsigned int failed);

	private:
		Ui::frmSummary ui;
};

#endif

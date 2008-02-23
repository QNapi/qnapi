/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __FRMABOUT__H__
#define __FRMABOUT__H__

#include <QDesktopWidget>

#include "ui_frmabout.h"
#include "version.h"

class frmAbout: public QDialog
{
Q_OBJECT
	public:
		frmAbout(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmAbout() {};
	private:
		Ui::frmAbout ui;
};

#endif

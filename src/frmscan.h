/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __FRMSCAN__H__
#define __FRMSCAN__H__

#include "ui_frmscan.h"
#include <QResizeEvent>

class frmScan: public QDialog
{
Q_OBJECT

	public:
		frmScan(QWidget *parent = 0, Qt::WFlags f = 0);
		~frmScan(){};

	private:
		Ui::frmScan ui;


	private slots:
		void resizeEvent(QResizeEvent *event);
};

#endif

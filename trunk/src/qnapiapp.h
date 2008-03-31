/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __QNAPIAPP__H__
#define __QNAPIAPP__H__

#include <QApplication>
#include <QString>
#include <QEvent>
#include <QFileOpenEvent>

#include "qcumber/qsingleapplication.h"

class QNapiApp : public QSingleApplication
{
	Q_OBJECT
	public:
		QNapiApp(int argc, char **argv) : QSingleApplication(argc, argv) {}

	signals:
		void downloadFile(const QString & fileName);

	private:
		bool event(QEvent *ev);
};

#endif

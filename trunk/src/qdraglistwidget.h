/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#ifndef __QDRAGLISTWIDGET__H__
#define __QDRAGLISTWIDGET__H__

#include <QtGui>

class QDragListWidget : public QListWidget
{
	Q_OBJECT
	public:
		QDragListWidget(QWidget *parent = 0) : QListWidget(parent)
		{
			setAcceptDrops(true);
		}

		void MoveAll(QDragListWidget *another);
		void MoveSelected(QDragListWidget *another);

	signals:
		void dragFinished();

	protected:
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);
		//virtual bool dropMimeData(int index, const QMimeData *data, Qt::DropAction action);
		QStringList mimeTypes() const;
		Qt::DropActions supportedDropActions () const;
		void mouseMoveEvent(QMouseEvent *event);

	private:
		QPoint dragStartPosition;
};

#endif

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
		QStringList mimeTypes() const;
		Qt::DropActions supportedDropActions () const;
		void mouseMoveEvent(QMouseEvent *event);

	private:
		QPoint dragStartPosition;
};

#endif

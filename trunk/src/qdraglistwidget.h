/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*****************************************************************************/

#ifndef __QDRAGLISTWIDGET__H__
#define __QDRAGLISTWIDGET__H__

#include <QtGui>

class QDragListWidget : public QListWidget
{
	Q_OBJECT
	public:
		QDragListWidget(QWidget *parent = 0, const QIcon & widgetIcon = QIcon())
			: QListWidget(parent)
		{
			setAcceptDrops(true);
			setWidgetIcon(widgetIcon);
		}

		void addItem(const QString & label);
		void addItem(QListWidgetItem * item);
		void addItems(const QStringList & labels);

		void setWidgetIcon(const QIcon & icon)
		{
			widgetIcon = icon;
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
		QIcon widgetIcon;
};

#endif

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

#ifndef __QCHECKEDLISTWIDGET__H__
#define __QCHECKEDLISTWIDGET__H__

#include <QtGui>
#include <QtDebug>


class QCheckedListWidget : public QListWidget
{
	public:

		QCheckedListWidget(QWidget * parent = 0) : QListWidget(parent)
		{
			setMouseTracking(true);
		}

		void setItemCheckState(int i, Qt::CheckState state)
		{
			QListWidgetItem * it = item(i);
			if(it) it->setCheckState(state);
		}

		Qt::CheckState itemCheckState(int i)
		{
			QListWidgetItem * it = item(i);
			return it ? it->checkState() : Qt::Unchecked;
		}

		void selectAll()
		{
			for(int i = 0; i < count(); ++i)
			{
				item(i)->setCheckState(Qt::Checked);
			}
		}

		void unselectAll()
		{
			for(int i = 0; i < count(); ++i)
			{
				item(i)->setCheckState(Qt::Unchecked);
			}
		}

		void invertSelection()
		{
			Qt::CheckState state;
			for(int i = 0; i < count(); ++i)
			{
				state = item(i)->checkState();
				state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
				item(i)->setCheckState(state);
			}
		}

	private:

		bool mouseUnderCheckBox;

		void checkMousePos(QPoint pos)
		{
			QListWidgetItem* item = itemAt(pos);

			if(!item)
			{
				mouseUnderCheckBox  = false;
				return;
			}

			QStyleOptionButton opt;
			opt.QStyleOption::operator=(viewOptions());
			opt.rect = visualItemRect(item);
			QRect r = style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt);

#ifdef Q_WS_WIN
			r.moveLeft(7);
#endif
#ifdef Q_WS_MAC
			r.moveLeft(10);
#endif

			mouseUnderCheckBox  = r.contains(pos);
		}

		void mouseMoveEvent(QMouseEvent *event)
		{
			checkMousePos(event->pos());
			QListWidget::mouseMoveEvent(event);
		}


		void mouseReleaseEvent(QMouseEvent *event)
		{
			checkMousePos(event->pos());
			
			QListWidgetItem* item = itemAt(event->pos());

			if(!item)
			{
				return;
			}

			if(!mouseUnderCheckBox)
				item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
			
			QListWidget::mouseReleaseEvent(event);
			//emit itemClicked(item);
		}

};


#endif // __QCHECKEDLISTWIDGET__H__

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

#ifndef __QNAPIENGINECONFIGMODEL__H__
#define __QNAPIENGINECONFIGMODEL__H__


#include <QAbstractTableModel>
#include "qnapi.h"


class QNapiEngineConfigModel : public QAbstractTableModel
{
Q_OBJECT	
	
	public:

		QNapiEngineConfigModel(QObject * parent = 0) : QAbstractTableModel(parent)
		{
			list["NapiProjekt"] = true;
			list["OpenSubtitles"] = true;
		}
		~QNapiEngineConfigModel(){};

		QVariant /*QNapiEngineConfigModel::*/headerData(int section, 
													Qt::Orientation orientation,
													int role) const
		{
			if (orientation == Qt::Horizontal)
			{
				if(role == Qt::DisplayRole && section == 0)
				{
					return "Silnik pobierania";
				}
			}
			return QAbstractTableModel::headerData(section, orientation, role);
		}


		QVariant /*QNapiEngineConfigModel::*/data(const QModelIndex &index,
												int role) const
		{
			if(!index.isValid()) return QVariant();													
			if((role == Qt::DisplayRole || role == Qt::EditRole)
			&& (index.column() == 0))
			{
				return list.keys().at(index.row());
			}
			else if (role == Qt::CheckStateRole && (index.column() == 0))
			{
				return list.values().at(index.row()) ? Qt::Checked : Qt::Unchecked;
			}
			else if (Qt::DecorationRole && (index.column() == 0))
			{
				QNapi n;
				n.addEngines(n.enumerateEngines());
				return n.engineByName(list.keys().at(index.row()))->engineIcon();
			}

			return QVariant();
		}


		bool setData(	const QModelIndex & index,
						const QVariant& value, int role)
		{
			if (!index.isValid()) return false;
			if (role == Qt::CheckStateRole && index.column() == 0)
			{
				QString key = data(index, Qt::DisplayRole).toString();
				list[key] = !list[key];
				emit dataChanged(index, index);
			}
		}
		
		Qt::ItemFlags flags(const QModelIndex &index) const
		{
			if (!index.isValid()) return 0;
			if (index.column() == 0)
				return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
			else
				return QAbstractTableModel::flags(index);
		}
		
		int columnCount(const QModelIndex &parent ) const
		{
			Q_UNUSED(parent);
			return 1;
		}


		int rowCount(const QModelIndex &parent ) const
		{
			Q_UNUSED(parent);
			return list.size();
		}

	private:
	
		QHash<QString, bool> list;

};


#endif

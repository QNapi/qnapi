/*****************************************************************************
** QNapi
** Copyright (C) 2008-2015 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "frmsummary.h"
#include "qnapi.h"

#include "subdatawidget.h"

frmSummary::frmSummary(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

void frmSummary::setSummaryList(QList<QNapiSubtitleInfo> &list)
{
    std::sort(list.begin(), list.end());

    QNapi n;
    n.addEngines(n.enumerateEngines());

    QIcon succIcon(":/ui/accept.png"), failIcon(":/ui/exclamation.png");

    ui.lwSummary->clear();
    ui.lwSummary->setFocusPolicy(Qt::NoFocus);

       int i = 0, good = 0, bad = 0;
        foreach(QNapiSubtitleInfo s, list)
        {
            bool isGood = false;

            if(s.resolution != SUBTITLE_NONE){
                ++good;
                isGood = true;
            }
            else{
                ++bad;
            }

            QNapiAbstractEngine *e = n.engineByName(s.engine);
            QListWidgetItem *listitem = new QListWidgetItem();


            ui.lwSummary->addItem(listitem);

            subDataWidget *subData = new subDataWidget();

            if(isGood){
                QString lang_path = QString(":/languages/") + s.lang + ".png";
                subData->setSubData(succIcon, s.name,QIcon(lang_path),e->engineIcon());

            }else{
                subData->setSubData(failIcon, QFileInfo(s.name).fileName());

            }
           ui.lwSummary->setItemWidget(listitem, subData);
           listitem->setSizeHint(subData->sizeHint());

            ++i;
        }


        ui.lwSummary->setMinimumWidth(ui.lwSummary->sizeHintForColumn(0) + 20);
        this->adjustSize();

        ui.lbSuccess->setVisible(good != 0);
        ui.lbFail->setVisible(bad != 0);

    ui.lbSuccess->setText(tr("Pobrano napisy dla %1 %2").arg(good).arg(tr(good > 1 ? "plików" : "pliku")));

    ui.lbFail->setText(tr("Nie pobrano napisów dla %1 %2").arg(bad).arg(tr(bad > 1 ? "plików" : "pliku")));
}

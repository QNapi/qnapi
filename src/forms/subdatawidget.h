#ifndef SUBDATAWIDGET_H
#define SUBDATAWIDGET_H

#include <QWidget>
#include "ui_subDataWidget.h"

class subDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit subDataWidget(QWidget *parent = 0);
    void setSubData(const QIcon &stateIcon, QString name, const QIcon &langIcon, const QIcon &engineIcon);
    void setSubData(const QIcon &stateIcon, QString name);
    void setSubData(QString name, QString ext, const QIcon &langIcon, const QIcon &engineIcon);
    void clearAll();
    ~subDataWidget();

private:
    Ui::subDataWidget *ui;
};

#endif // SUBDATAWIDGET_H

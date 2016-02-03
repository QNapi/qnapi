#ifndef SUBDATAWIDGET_H
#define SUBDATAWIDGET_H

#include <QWidget>

namespace Ui {
class subDataWidget;
}

class subDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit subDataWidget(QWidget *parent = 0);
    void setSubData(const QIcon &_stateIcon, QString _name, const QIcon &_langIcon, const QIcon &_engineIcon);
    void setSubData(const QIcon &_stateIcon, QString _name);
    ~subDataWidget();

private:
    Ui::subDataWidget *ui;
};

#endif // SUBDATAWIDGET_H

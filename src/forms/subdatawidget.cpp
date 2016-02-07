#include "subdatawidget.h"

subDataWidget::subDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subDataWidget)
{
    ui->setupUi(this);
}

subDataWidget::~subDataWidget()
{
    delete ui;
}

void subDataWidget::setSubData(const QIcon &stateIcon, QString name, const QIcon &langIcon, const QIcon &engineIcon){

    ui->place1->clear();
    ui->place2->clear();
    ui->place3->clear();
    ui->place4->clear();

    ui->place1->setPixmap(stateIcon.pixmap(16));
    ui->place2->setText(name);
    ui->place3->setPixmap(langIcon.pixmap(16));
    ui->place4->setPixmap(engineIcon.pixmap(16));
}

void subDataWidget::setSubData(const QIcon &stateIcon, QString name){

    ui->place1->clear();
    ui->place2->clear();
    ui->place3->clear();
    ui->place4->clear();

    ui->place3->hide();
    ui->place4->hide();

    ui->place1->setPixmap(stateIcon.pixmap(16));
    ui->place2->setText(name);
}

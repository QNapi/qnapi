#ifndef __FRMCONVERT_H__
#define __FRMCONVERT_H__

#include "ui_frmconvert.h"
#include <QDialog>

class frmConvert : public QDialog
{
Q_OBJECT
public:
    frmConvert(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~frmConvert() {}
private:
    Ui::frmConvert ui;
};

#endif // __FRMCONVERT_H__

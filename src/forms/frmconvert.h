#ifndef __FRMCONVERT_H__
#define __FRMCONVERT_H__

#include "ui_frmconvert.h"
#include <QDialog>
#include "subconvert/subtitleconverter.h"

class frmConvert : public QDialog
{
Q_OBJECT
public:
    frmConvert(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~frmConvert() {}
private:
    Ui::frmConvert ui;
    SubtitleConverter subConverter;
    QString srcFormat, targetFormat;
    bool targetFileNameSelected;

    void checkFPSNeeded();
    QString determineMovieFPS(const QString & defaultMovieFilePath);
    void generateTargetFileName();

private slots:

    void srcSubSelectClicked();
    void srcSubFileLoaded(const QString & srcSubFileName);
    void targetFormatChanged(int targetFormatIdx);
    void movieFPSSelectClicked();
    void targetMovieFPSSelectClicked();
    void targetExtensionChanged();
    void subDelayToggled();
    void convertClicked();
};

#endif // __FRMCONVERT_H__

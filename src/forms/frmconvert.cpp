#include "frmconvert.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "subconvert/subtitleformatsregistry.h"
#include "ffprobemovieinfoparser.h"
#include "qnapiconfig.h"
#include "qnapiopendialog.h"

frmConvert::frmConvert(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), targetFileNameSelected(false)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, false);

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());

    ui.lbDetectedFormatValue->setText("");

    ui.cbTargetFormat->clear();
    foreach(QString format, GlobalFormatsRegistry().enumerateFormats())
    {
        ui.cbTargetFormat->addItem(format);
    }

    connect(ui.pbSrcSubFileSelect, SIGNAL(clicked()), this, SLOT(srcSubSelectClicked()));
    connect(ui.leSrcSubFile, SIGNAL(textChanged(const QString &)), this, SLOT(srcSubFileLoaded(const QString &)));
    connect(ui.cbTargetFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(targetFormatChanged(int)));
    connect(ui.cbTargetExtension, SIGNAL(currentIndexChanged(int)), this, SLOT(targetExtensionChanged()));
    connect(ui.pbMovieFPSSelect, SIGNAL(clicked()), this, SLOT(movieFPSSelectClicked()));
    connect(ui.pbTargetMovieFPSSelect, SIGNAL(clicked()), this, SLOT(targetMovieFPSSelectClicked()));
    connect(ui.pbConvert, SIGNAL(clicked()), this, SLOT(convertClicked()));

    if(GlobalConfig().ppSubFormat().isEmpty())
    {
        targetFormat = GlobalFormatsRegistry().enumerateFormats().first();
    }
    else
    {
        targetFormat = GlobalConfig().ppSubFormat();
        ui.cbTargetFormat->setCurrentText(targetFormat);
    }

    if(!GlobalConfig().ppSubExtension().isEmpty())
    {
        ui.cbTargetExtension->setCurrentText(GlobalConfig().ppSubExtension());
    }
}

void frmConvert::srcSubSelectClicked()
{
    QNapiOpenDialog openSubtitle(this, tr("Wybierz plik z napisami"),
                                 QFileInfo(ui.leSrcSubFile->text()).path(),
                                 QNapiOpenDialog::Subtitles);
    if(openSubtitle.selectFile())
    {
        ui.leSrcSubFile->setText(openSubtitle.selectedFiles().first());
    }
}

void frmConvert::srcSubFileLoaded(const QString & srcSubFileName)
{
    bool fileExists = QFileInfo(srcSubFileName).exists();

    ui.lbDetectedFormat->setEnabled(fileExists);
    ui.lbDetectedFormatValue->setVisible(fileExists);

    if(fileExists)
    {
        QString detectedFormat = subConverter.detectFormat(srcSubFileName);
        if(detectedFormat.isEmpty())
        {
            ui.lbDetectedFormatValue->setText(tr("niepoprawny"));
            ui.lbDetectedFormatValue->setStyleSheet("QLabel { color: red }");
        } else {
            ui.lbDetectedFormatValue->setText(detectedFormat);
            ui.lbDetectedFormatValue->setStyleSheet("QLabel { color: black }");
        }
        srcFormat = detectedFormat;
    } else {
        srcFormat = "";
    }

    ui.lbTargetFormat->setEnabled(!srcFormat.isEmpty());
    ui.cbTargetFormat->setEnabled(!srcFormat.isEmpty());
    ui.lbTargetExtension->setEnabled(!srcFormat.isEmpty());
    ui.cbTargetExtension->setEnabled(!srcFormat.isEmpty());
    ui.cbChangeFPS->setEnabled(!srcFormat.isEmpty());
    ui.pbMovieFPSSelect->setEnabled(!srcFormat.isEmpty());
    ui.lbTargetFileName->setEnabled(!srcFormat.isEmpty());
    ui.leTargetFileName->setEnabled(!srcFormat.isEmpty());
    ui.pbConvert->setEnabled(!srcFormat.isEmpty());

    anyFormatChanged();
    generateTargetFileName();
}


void frmConvert::targetFormatChanged(int targetFormatIdx)
{
    targetFormat = GlobalFormatsRegistry().enumerateFormats().at(targetFormatIdx);
    anyFormatChanged();
}

void frmConvert::anyFormatChanged()
{
    bool fpsNeeded;
    if(srcFormat.isEmpty() || targetFormat.isEmpty())
    {
        fpsNeeded = false;
    } else {
        SubtitleFormat * srcSF = GlobalFormatsRegistry().select(srcFormat);
        SubtitleFormat * targetSF = GlobalFormatsRegistry().select(targetFormat);
        fpsNeeded = srcSF->isTimeBased() != targetSF->isTimeBased();

        QString targetDefaultExt = targetSF->defaultExtension();
        ui.cbTargetExtension->setItemText(0, tr("Domyślne (%1)").arg(targetDefaultExt));
        generateTargetFileName();
    }

    ui.cbMovieFPS->setEnabled(fpsNeeded);
    ui.lbMovieFPS->setEnabled(fpsNeeded);
    ui.pbMovieFPSSelect->setEnabled(fpsNeeded);

    if(fpsNeeded) {
        QFileInfo srcSubFI(ui.leSrcSubFile->text());
        QString defaultMovieFilePath = srcSubFI.absolutePath() + QDir::separator() + srcSubFI.completeBaseName() + ".avi";
        if(QFileInfo(defaultMovieFilePath).exists())
        {
            QString fps = determineMovieFPS(defaultMovieFilePath);
            ui.cbMovieFPS->setCurrentText(fps);
            ui.cbFPSTo->setCurrentText(fps);
        }
    }
}

void frmConvert::movieFPSSelectClicked()
{
    QNapiOpenDialog openMovie(this, tr("Wybierz plik z filmem"),
                              QFileInfo(ui.leSrcSubFile->text()).path(),
                              QNapiOpenDialog::Movies);
    if(openMovie.selectFile())
    {
        QString moviePath = openMovie.selectedFiles().first();
        QString fps = determineMovieFPS(moviePath);
        ui.cbMovieFPS->setCurrentText(fps);
    }
}

QString frmConvert::determineMovieFPS(const QString & movieFilePath)
{
    FFProbeMovieInfoParser mip(GlobalConfig().ffProbePath());
    MovieInfo mi = mip.parseFile(movieFilePath);
    return QString::number(mi.frameRate, 'f', 3);
}

void frmConvert::targetMovieFPSSelectClicked()
{
    QNapiOpenDialog openMovie(this, tr("Wybierz plik z filmem"),
                              QFileInfo(ui.leSrcSubFile->text()).path(),
                              QNapiOpenDialog::Movies);
    if(openMovie.selectFile())
    {
        QString moviePath = openMovie.selectedFiles().first();
        QString fps = determineMovieFPS(moviePath);
        ui.cbFPSTo->setCurrentText(fps);
    }
}

void frmConvert::targetExtensionChanged()
{
    generateTargetFileName();
}

void frmConvert::generateTargetFileName()
{
    if(!targetFileNameSelected)
    {
        QFileInfo srcSubFI(ui.leSrcSubFile->text());
        QString extension;

        if(ui.cbTargetExtension->currentIndex() == 0)
        {
            SubtitleFormat * targetSF = GlobalFormatsRegistry().select(targetFormat);
            extension = targetSF->defaultExtension();
        } else {
            extension = ui.cbTargetExtension->currentText();
        }

        QString defaultTargetPath = srcSubFI.absolutePath() + QDir::separator() + srcSubFI.completeBaseName() + "." + extension;
        ui.leTargetFileName->setText(defaultTargetPath);
    }
}

void frmConvert::convertClicked()
{
    double fpsRatio = 1.0;
    if(ui.cbChangeFPS->isChecked())
    {
        double fpsFrom = ui.cbFPSFrom->currentText().toDouble();
        double fpsTo = ui.cbFPSTo->currentText().toDouble();
        fpsRatio = fpsTo / fpsFrom;
    }

    if(subConverter.convertSubtitles(ui.leSrcSubFile->text(),
                                     targetFormat,
                                     ui.leTargetFileName->text(),
                                     ui.cbMovieFPS->currentText().toDouble(),
                                     fpsRatio))
    {
        QMessageBox::information(this, tr("Przekonwertowano napisy"),
                                 tr("Poprawnie zmieniono format napisów z '%1' na '%2'").arg(srcFormat, targetFormat));

    } else {
        QMessageBox::warning(this, tr("Problem podczas konwertowania napisów"),
                                 tr("Wystąpił problem podczas zamiany formatu napisów!"));
    }
}

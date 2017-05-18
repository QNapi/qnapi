/*****************************************************************************
** QNapi
** Copyright (C) 2008-2017 Piotr Krzemiński <pio.krzeminski@gmail.com>
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

#include "frmconvert.h"
#include "libqnapi.h"
#include "movieinfo/movieinfoprovider.h"
#include "qnapiopendialog.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSharedPointer>

frmConvert::frmConvert(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      staticConfig(LibQNapi::staticConfig()),
      ppConfig(LibQNapi::loadConfig().postProcessingConfig()),
      subtitleFormatsRegistry(LibQNapi::subtitleFormatsRegistry()),
      subConverter(subtitleFormatsRegistry, LibQNapi::movieInfoProvider(),
                   ppConfig.skipConvertAds()),
      targetFileNameSelected(false) {
  ui.setupUi(this);

  setAttribute(Qt::WA_QuitOnClose, false);

  QRect position = frameGeometry();
  position.moveCenter(QDesktopWidget().availableGeometry().center());
  move(position.topLeft());

  ui.lbDetectedFormatValue->setText("");

  ui.cbTargetFormat->clear();
  foreach (QString format, subtitleFormatsRegistry->listFormatNames()) {
    ui.cbTargetFormat->addItem(format);
  }

  connect(ui.pbSrcSubFileSelect, SIGNAL(clicked()), this,
          SLOT(srcSubSelectClicked()));
  connect(ui.leSrcSubFile, SIGNAL(textChanged(const QString &)), this,
          SLOT(srcSubFileLoaded(const QString &)));
  connect(ui.cbTargetFormat, SIGNAL(currentIndexChanged(int)), this,
          SLOT(targetFormatChanged(int)));
  connect(ui.cbTargetExtension, SIGNAL(currentIndexChanged(int)), this,
          SLOT(targetExtensionChanged()));
  connect(ui.pbMovieFPSSelect, SIGNAL(clicked()), this,
          SLOT(movieFPSSelectClicked()));
  connect(ui.pbTargetMovieFPSSelect, SIGNAL(clicked()), this,
          SLOT(targetMovieFPSSelectClicked()));
  connect(ui.cbDelaySubtitles, SIGNAL(toggled(bool)), this,
          SLOT(subDelayToggled()));
  connect(ui.pbConvert, SIGNAL(clicked()), this, SLOT(convertClicked()));

  if (ppConfig.subFormat().isEmpty()) {
    targetFormat = subtitleFormatsRegistry->listFormatNames().first();
  } else {
    targetFormat = ppConfig.subFormat();
    ui.cbTargetFormat->setCurrentText(targetFormat);
  }

  if (!ppConfig.subExtension().isEmpty()) {
    ui.cbTargetExtension->setCurrentText(ppConfig.subExtension());
  }
}

void frmConvert::srcSubSelectClicked() {
  QNapiOpenDialog openSubtitle(this, tr("Choose a subtitles file"),
                               QFileInfo(ui.leSrcSubFile->text()).path(),
                               QNapiOpenDialog::Subtitles);
  if (openSubtitle.selectFile()) {
    ui.leSrcSubFile->setText(openSubtitle.selectedFiles().first());
  }
}

void frmConvert::srcSubFileLoaded(const QString &srcSubFileName) {
  bool fileExists = QFileInfo(srcSubFileName).exists();

  ui.lbDetectedFormat->setEnabled(fileExists);
  ui.lbDetectedFormatValue->setVisible(fileExists);

  if (fileExists) {
    QString detectedFormat = subConverter.detectFormat(srcSubFileName);
    if (detectedFormat.isEmpty()) {
      ui.lbDetectedFormatValue->setText(tr("incorrect"));
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
  ui.cbDelaySubtitles->setEnabled(!srcFormat.isEmpty());
  ui.pbMovieFPSSelect->setEnabled(!srcFormat.isEmpty());
  ui.lbTargetFileName->setEnabled(!srcFormat.isEmpty());
  ui.leTargetFileName->setEnabled(!srcFormat.isEmpty());
  ui.pbConvert->setEnabled(!srcFormat.isEmpty());

  checkFPSNeeded();
  generateTargetFileName();
}

void frmConvert::targetFormatChanged(int targetFormatIdx) {
  targetFormat = subtitleFormatsRegistry->listFormatNames().at(targetFormatIdx);
  checkFPSNeeded();
}

void frmConvert::checkFPSNeeded() {
  bool fpsNeeded;
  if (srcFormat.isEmpty() || targetFormat.isEmpty()) {
    fpsNeeded = false;
  } else {
    QSharedPointer<const SubtitleFormat> srcSF =
        subtitleFormatsRegistry->select(srcFormat);
    QSharedPointer<const SubtitleFormat> targetSF =
        subtitleFormatsRegistry->select(targetFormat);
    fpsNeeded = (srcSF->isTimeBased() != targetSF->isTimeBased()) ||
                (ui.cbDelaySubtitles->isChecked() && !targetSF->isTimeBased());

    QString targetDefaultExt = targetSF->defaultExtension();
    ui.cbTargetExtension->setItemText(0,
                                      tr("Default (%1)").arg(targetDefaultExt));
    generateTargetFileName();
  }

  ui.cbMovieFPS->setEnabled(fpsNeeded);
  ui.lbMovieFPS->setEnabled(fpsNeeded);
  ui.pbMovieFPSSelect->setEnabled(fpsNeeded);

  if (fpsNeeded) {
    QFileInfo srcSubFI(ui.leSrcSubFile->text());

    QString movieFilePathBase =
        srcSubFI.absolutePath() + "/" + srcSubFI.completeBaseName();

    foreach (QString movieExt, staticConfig->movieExtensions()) {
      QString movieFilePath = movieFilePathBase + "." + movieExt;
      if (QFileInfo(movieFilePath).exists()) {
        Maybe<QString> maybeFPS = determineMovieFPS(movieFilePath);
        if (maybeFPS) {
          ui.cbMovieFPS->setCurrentText(maybeFPS.value());
          ui.cbFPSTo->setCurrentText(maybeFPS.value());
        }
        break;
      }
    }
  }
}

void frmConvert::movieFPSSelectClicked() {
  QNapiOpenDialog openMovie(this, tr("Select a video file"),
                            QFileInfo(ui.leSrcSubFile->text()).path(),
                            QNapiOpenDialog::Movies);
  if (openMovie.selectFile()) {
    QString moviePath = openMovie.selectedFiles().first();
    Maybe<QString> maybeFPS = determineMovieFPS(moviePath);
    if (maybeFPS) {
      ui.cbMovieFPS->setCurrentText(maybeFPS.value());
    }
  }
}

Maybe<QString> frmConvert::determineMovieFPS(const QString &movieFilePath) {
  QSharedPointer<const MovieInfoProvider> mip = LibQNapi::movieInfoProvider();
  Maybe<MovieInfo> mmi = mip->getMovieInfo(movieFilePath);
  if (mmi)
    return just(QString::number(mmi.value().frameRate(), 'f', 3));
  else
    return nothing();
}

void frmConvert::targetMovieFPSSelectClicked() {
  QNapiOpenDialog openMovie(this, tr("Select a video file"),
                            QFileInfo(ui.leSrcSubFile->text()).path(),
                            QNapiOpenDialog::Movies);
  if (openMovie.selectFile()) {
    QString moviePath = openMovie.selectedFiles().first();
    Maybe<QString> maybeFPS = determineMovieFPS(moviePath);
    if (maybeFPS) {
      ui.cbFPSTo->setCurrentText(maybeFPS.value());
    }
  }
}

void frmConvert::targetExtensionChanged() { generateTargetFileName(); }

void frmConvert::generateTargetFileName() {
  if (!targetFileNameSelected) {
    QFileInfo srcSubFI(ui.leSrcSubFile->text());
    QString extension;

    if (ui.cbTargetExtension->currentIndex() == 0) {
      QSharedPointer<const SubtitleFormat> targetSF =
          subtitleFormatsRegistry->select(targetFormat);
      extension = targetSF->defaultExtension();
    } else {
      extension = ui.cbTargetExtension->currentText();
    }

    QString defaultTargetPath = srcSubFI.absolutePath() + "/" +
                                srcSubFI.completeBaseName() + "." + extension;
    ui.leTargetFileName->setText(defaultTargetPath);
  }
}

void frmConvert::subDelayToggled() { checkFPSNeeded(); }

void frmConvert::convertClicked() {
  double fpsRatio = 1.0, delayOffset = 0.0;

  if (ui.cbChangeFPS->isChecked()) {
    double fpsFrom = ui.cbFPSFrom->currentText().replace(',', '.').toDouble();
    double fpsTo = ui.cbFPSTo->currentText().replace(',', '.').toDouble();
    fpsRatio = fpsTo / fpsFrom;
  }

  if (ui.cbDelaySubtitles->isChecked()) {
    delayOffset = ui.sbDelayOffset->value();
  }

  if (subConverter.convertSubtitles(
          ui.leSrcSubFile->text(), targetFormat, ui.leTargetFileName->text(),
          ui.cbMovieFPS->currentText().replace(',', '.').toDouble(), fpsRatio,
          delayOffset)) {
    QMessageBox::information(this, tr("Subtitles converted"),
                             tr("Changed subtitles format from '%1' to '%2'")
                                 .arg(srcFormat, targetFormat));
  } else {
    QMessageBox::warning(this, tr("Could not change subtitle format!"),
                         tr("An error occured while converting subtitles!"));
  }
}

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

#include "frmoptions.h"
#include "qnapiapp.h"
#include "subconvert/subtitleformatsregistry.h"


frmOptions::frmOptions(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui.setupUi(this);

#ifdef Q_OS_MAC
    ui.cbQuietBatch->hide();
#endif

#ifndef Q_OS_MAC
    ui.cbShowDockIcon->hide();
#endif


#ifdef Q_OS_WIN
    // Pod Windowsami chowamy kontrolki odpowiadajace za zmiane uprawnien - i tak sie nie przydadza
    ui.cbChangePermissions->hide();
    ui.sbOPerm->hide();
    ui.sbGPerm->hide();
    ui.sbUPerm->hide();
#endif

    QString tlcode;
    ui.cbLangBackup->addItem("None", QVariant(""));
    foreach(QString lang, QNapiLanguage("").listLanguages())
    {
        tlcode = QNapiLanguage(lang).toTwoLetter();
        ui.cbLang->addItem(QIcon(QString(":/languages/%1.png").arg(tlcode)),lang,QVariant(tlcode));
        ui.cbLangBackup->addItem(QIcon(QString(":/languages/%1.png").arg(tlcode)),lang,QVariant(tlcode));
    }

    setAttribute(Qt::WA_QuitOnClose, false);

    connect(ui.le7zPath, SIGNAL(textChanged(const QString &)), this, SLOT(le7zPathChanged()));
    connect(ui.pb7zPathSelect, SIGNAL(clicked()), this, SLOT(select7zPath()));
    connect(ui.leTmpPath, SIGNAL(textChanged(const QString &)), this, SLOT(leTmpPathChanged()));
    connect(ui.pbTmpPathSelect, SIGNAL(clicked()), this, SLOT(selectTmpPath()));

    connect(ui.twEngines, SIGNAL(itemSelectionChanged()), this, SLOT(twEnginesSelectionChanged()));
    connect(ui.twEngines, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(twEnginesItemChanged(QTableWidgetItem *)));

    connect(ui.pbMoveUp, SIGNAL(clicked()), this, SLOT(pbMoveUpClicked()));
    connect(ui.pbMoveDown, SIGNAL(clicked()), this, SLOT(pbMoveDownClicked()));
    connect(ui.pbEngineConf, SIGNAL(clicked()), this, SLOT(pbEngineConfClicked()));
    connect(ui.pbEngineInfo, SIGNAL(clicked()), this, SLOT(pbEngineInfoClicked()));

    connect(ui.cbSubFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(subFormatChanged(int)));
    connect(ui.cbEncodingMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(encodingMethodChanged(int)));
    connect(ui.cbAutoDetectEncoding, SIGNAL(clicked()), this, SLOT(autoDetectEncodingClicked()));
    connect(ui.cbShowAllEncodings, SIGNAL(clicked()), this, SLOT(showAllEncodingsClicked()));

    connect(ui.pbRestoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    showAllEncodings();

    foreach(QString format, GlobalFormatsRegistry().enumerateFormats())
    {
        ui.cbSubFormat->addItem(format);
    }

    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());
}

frmOptions::~frmOptions()
{
}

void frmOptions::le7zPathChanged()
{
    QFileInfo f(ui.le7zPath->text());
    ui.le7zPath->setStyleSheet(
        f.isFile() && f.isExecutable()
            ? ""
            : "color:red;"
        );
}

void frmOptions::select7zPath()
{
    QString path7z = QFileDialog::getOpenFileName(this, tr("Specify the path for 7z executable"),
                                                    QFileInfo(ui.le7zPath->text()).path());
    if(!path7z.isEmpty())
    {
        if(!QFileInfo(path7z).isExecutable())
            QMessageBox::warning(this, tr("Invalid path"),
                tr("Defined path to 7z executable is invalid. If you can not "
                    "locate 7z executable, try installing p7zip-full package."));
        else
            ui.le7zPath->setText(path7z);
    }
}

void frmOptions::leTmpPathChanged()
{
    QFileInfo f(ui.leTmpPath->text());
    ui.leTmpPath->setStyleSheet(
        f.isDir() && f.isWritable()
            ? ""
            : "color:red;"
        );
}

void frmOptions::selectTmpPath()
{
    QString tmpDir = QFileDialog::getExistingDirectory(this,
                                    tr("Specify temporary directory"),
                                    QFileInfo(ui.leTmpPath->text()).path(),
                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!tmpDir.isEmpty())
        ui.leTmpPath->setText(QFileInfo(tmpDir).path());
}

void frmOptions::twEnginesSelectionChanged()
{
    QNapi n;
    n.addEngines(n.enumerateEngines());

    if(ui.twEngines->selectedItems().size() < 1)
        return; 

    int currentRow = ui.twEngines->row(ui.twEngines->selectedItems().at(0));

    ui.pbMoveUp->setEnabled(currentRow > 0);
    ui.pbMoveDown->setEnabled(currentRow < ui.twEngines->rowCount() - 1);
    ui.pbEngineConf->setEnabled(true);
    ui.pbEngineInfo->setEnabled(true);
}

void frmOptions::twEnginesItemChanged(QTableWidgetItem * item)
{
    bool foundActive = false;

    for(int i = 0; i < ui.twEngines->rowCount(); ++i)
    {
        if(!ui.twEngines->item(i, 0))
        {
            return;
        }

        if(ui.twEngines->item(i, 0)->checkState() == Qt::Checked)
        {
            foundActive = true;
            break;
        }
    }

    if(!foundActive)
    {
        item->setCheckState(Qt::Checked);
        QMessageBox::warning(this,
                            "Warning",
                            "At least one subtitle engine must remain active!");
    }
    
}

void frmOptions::pbMoveUpClicked()
{
    int currentRow = ui.twEngines->row(ui.twEngines->selectedItems().at(0));

    QTableWidgetItem *current, *above;
    current = ui.twEngines->item(currentRow, 0);
    above = ui.twEngines->item(currentRow - 1, 0);

    QTableWidgetItem tmp = *current;
    *current = *above;
    *above = tmp;

    ui.twEngines->selectRow(currentRow - 1);
}

void frmOptions::pbMoveDownClicked()
{
    int currentRow = ui.twEngines->row(ui.twEngines->selectedItems().at(0));

    QTableWidgetItem *current, *below;
    current = ui.twEngines->item(currentRow, 0);
    below = ui.twEngines->item(currentRow + 1, 0);

    QTableWidgetItem tmp = *current;
    *current = *below;
    *below = tmp;

    ui.twEngines->selectRow(currentRow + 1);
}

void frmOptions::pbEngineConfClicked()
{
    QNapi n;
    QString engineName = ui.twEngines->selectedItems().at(0)->text();
    n.configureEngine(engineName, this);
}

void frmOptions::pbEngineInfoClicked()
{
    QNapi n;
    n.addEngines(n.enumerateEngines());
    QString engineName = ui.twEngines->selectedItems().at(0)->text();
    QString engineInfo = n.engineByName(engineName)->engineInfo();
    
    QMessageBox::information(this,
                             QString("%1 subtitle engine info").arg(engineName),
                             engineInfo);
}

void frmOptions::subFormatChanged(int format)
{
    if(format == 0)
    {
        ui.cbSubExtension->setItemText(0, tr("Default"));
    }
    else
    {
        QString targetFormatName = ui.cbSubFormat->currentText();
        SubtitleFormat * targetSF = GlobalFormatsRegistry().select(targetFormatName);
        QString targetDefaultExt = targetSF->defaultExtension();
        ui.cbSubExtension->setItemText(0, tr("Default (%1)").arg(targetDefaultExt));
    }
}

void frmOptions::encodingMethodChanged(int method)
{
    ChangeEncodingMethod cemMethod = (ChangeEncodingMethod) method;
    bool enableEncodingSettings = cemMethod == CEM_CHANGE;
    ui.cbEncFrom->setEnabled(enableEncodingSettings);
    ui.lbConvert->setEnabled(enableEncodingSettings);
    ui.lbConvertFrom->setEnabled(enableEncodingSettings);
    ui.cbEncTo->setEnabled(enableEncodingSettings);
    ui.cbAutoDetectEncoding->setEnabled(enableEncodingSettings);
    ui.cbShowAllEncodings->setEnabled(enableEncodingSettings);
    autoDetectEncodingClicked();
}

void frmOptions::autoDetectEncodingClicked()
{
    bool checkedCE = (ChangeEncodingMethod) ui.cbEncodingMethod->currentIndex() == CEM_CHANGE;
    bool checkedADE = ui.cbAutoDetectEncoding->isChecked();
    ui.cbEncFrom->setEnabled(checkedCE && !checkedADE);
    ui.lbConvertFrom->setEnabled(checkedCE && !checkedADE);
}

void frmOptions::showAllEncodingsClicked()
{
    QString encFrom = ui.cbEncFrom->currentText();
    QString encTo = ui.cbEncTo->currentText();

    if(ui.cbShowAllEncodings->isChecked())
    {
        showAllEncodings();
    }
    else
    {
        ui.cbEncFrom->clear();
        ui.cbEncTo->clear();

        QStringList codecs;
        codecs << "windows-1250" << "windows-1257" << "ISO-8859-2" << "ISO-8859-13"
                << "ISO-8859-16" << "UTF-8";

        ui.cbEncFrom->addItems(codecs);
        ui.cbEncTo->addItems(codecs);
    }

    ui.cbEncFrom->setCurrentIndex(ui.cbEncFrom->findText(encFrom));
    ui.cbEncTo->setCurrentIndex(ui.cbEncTo->findText(encTo));
}

void frmOptions::showAllEncodings()
{
    ui.cbEncFrom->clear();
    ui.cbEncTo->clear();

    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    qSort(codecs.begin(), codecs.end());
    for(QList<QByteArray>::iterator i = codecs.begin(); i != codecs.end(); i++)
    {
        ui.cbEncFrom->addItem(*i);
        ui.cbEncTo->addItem(*i);
    }
}

void frmOptions::writeConfig()
{
    GlobalConfig().setP7zipPath(ui.le7zPath->text());
    GlobalConfig().setTmpPath(ui.leTmpPath->text());
    GlobalConfig().setLanguage(ui.cbLang->itemData(ui.cbLang->currentIndex()).toString());
    GlobalConfig().setLanguageBackup(ui.cbLangBackup->itemData(ui.cbLangBackup->currentIndex()).toString());
    GlobalConfig().setNoBackup(ui.cbNoBackup->isChecked());
#ifndef Q_OS_MAC
    GlobalConfig().setQuietBatch(ui.cbQuietBatch->isChecked());
#endif

#ifdef Q_OS_MAC
    GlobalConfig().setShowDockIcon(ui.cbShowDockIcon->isChecked());
#endif

    QList<QPair<QString, bool> > engines;
    for(int i = 0; i < ui.twEngines->rowCount(); ++i)
    {
        engines << qMakePair(ui.twEngines->item(i, 0)->text(),
                            (ui.twEngines->item(i, 0)->checkState() == Qt::Checked));
    }

    GlobalConfig().setEngines(engines);
    
    GlobalConfig().setSearchPolicy((SearchPolicy) ui.cbSearchPolicy->currentIndex());
    GlobalConfig().setDownloadPolicy((DownloadPolicy) ui.cbDownloadPolicy->currentIndex());

    GlobalConfig().setPpEnabled(ui.gbPpEnable->isChecked());
    GlobalConfig().setPpEncodingMethod((ChangeEncodingMethod) ui.cbEncodingMethod->currentIndex());
    GlobalConfig().setPpAutoDetectEncoding(ui.cbAutoDetectEncoding->isChecked());
    GlobalConfig().setPpEncodingFrom(ui.cbEncFrom->currentText());
    GlobalConfig().setPpEncodingTo(ui.cbEncTo->currentText());
    GlobalConfig().setPpShowAllEncodings(ui.cbShowAllEncodings->isChecked());
    GlobalConfig().setPpRemoveLines(ui.cbRemoveLines->isChecked());
    GlobalConfig().setPpRemoveWords(ui.teRemoveWords->toPlainText().split("\n"));
    QString targetFormat = ui.cbSubFormat->currentIndex() == 0 ? "" : GlobalFormatsRegistry().enumerateFormats().at(ui.cbSubFormat->currentIndex() - 1);
    GlobalConfig().setPpSubFormat(targetFormat);
    QString targetExt = ui.cbSubExtension->currentIndex() == 0 ? "" : ui.cbSubExtension->currentText();
    GlobalConfig().setPpSubExtension(targetExt);
    GlobalConfig().setPpSkipConvertAds(ui.cbSkipConvertAds->isChecked());
    GlobalConfig().setChangePermissions(ui.cbChangePermissions->isChecked());

    QString permissions = QString("%1%2%3").arg(ui.sbUPerm->value())
                                           .arg(ui.sbGPerm->value())
                                           .arg(ui.sbOPerm->value());
    GlobalConfig().setChangePermissionsTo(permissions);

    GlobalConfig().save();
}

void frmOptions::readConfig()
{
    GlobalConfig().load(QCoreApplication::applicationDirPath());

    ui.le7zPath->setText(GlobalConfig().p7zipPath());
    ui.leTmpPath->setText(GlobalConfig().tmpPath());
    ui.cbLang->setCurrentIndex(ui.cbLang->findData(QNapiLanguage(GlobalConfig().language()).toTwoLetter()));
    ui.cbLangBackup->setCurrentIndex(ui.cbLangBackup->findData(QNapiLanguage(GlobalConfig().languageBackup()).toTwoLetter()));
    ui.cbNoBackup->setChecked(GlobalConfig().noBackup());
#ifndef Q_OS_MAC
    ui.cbQuietBatch->setChecked(GlobalConfig().quietBatch());
#endif
#ifdef Q_OS_MAC
    ui.cbShowDockIcon->setChecked(GlobalConfig().showDockIcon());
#endif

    QNapi n;
    n.addEngines(n.enumerateEngines());

    ui.twEngines->clear();

    QList<QPair<QString,bool> > engines = GlobalConfig().engines();
    ui.twEngines->setColumnCount(1);
    ui.twEngines->setRowCount(engines.size());

    for(int i = 0; i < engines.size(); ++i)
    {
        QPair<QString,bool> e = engines.at(i);
        QTableWidgetItem *item = new QTableWidgetItem(QIcon(QPixmap(n.engineByName(e.first)->enginePixmapData())), e.first);
        item->setCheckState(e.second ? Qt::Checked : Qt::Unchecked);
        ui.twEngines->setItem(i, 0, item);
    }

    ui.twEngines->horizontalHeader()->hide();
    ui.twEngines->verticalHeader()->hide();
    ui.twEngines->verticalHeader()->setDefaultSectionSize(20);
    ui.twEngines->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui.twEngines->setColumnWidth(0, 300);

    ui.cbSearchPolicy->setCurrentIndex(GlobalConfig().searchPolicy());
    ui.cbDownloadPolicy->setCurrentIndex(GlobalConfig().downloadPolicy());

    ui.cbEncodingMethod->setCurrentIndex(GlobalConfig().ppEncodingMethod());
    ui.cbAutoDetectEncoding->setChecked(GlobalConfig().ppAutoDetectEncoding());
    ui.cbEncFrom->setCurrentIndex(ui.cbEncFrom->findText(GlobalConfig().ppEncodingFrom()));
    ui.cbEncTo->setCurrentIndex(ui.cbEncTo->findText(GlobalConfig().ppEncodingTo()));
    ui.cbShowAllEncodings->setChecked(GlobalConfig().ppShowAllEncodings());
    ui.cbRemoveLines->setChecked(GlobalConfig().ppRemoveLines());
    ui.teRemoveWords->setText(GlobalConfig().ppRemoveWords().join("\n"));

    int formatIdx = 1;
    foreach(QString format, GlobalFormatsRegistry().enumerateFormats())
    {
        if(GlobalConfig().ppSubFormat() == format)
        {
            ui.cbSubFormat->setCurrentIndex(formatIdx);
        }
        ++formatIdx;
    }

    ui.cbSubExtension->setCurrentText(GlobalConfig().ppSubExtension());
    ui.cbSkipConvertAds->setChecked(GlobalConfig().ppSkipConvertAds());

    ui.cbChangePermissions->setChecked(GlobalConfig().changePermissions());
    QString permissions = GlobalConfig().changePermissionsTo();
    unsigned short o, g, u;
    o = permissions.at(0).toLatin1() - '0';
    g = permissions.at(1).toLatin1() - '0';
    u = permissions.at(2).toLatin1() - '0';
    ui.sbUPerm->setValue((o <= 7) ? o : 6);
    ui.sbGPerm->setValue((g <= 7) ? g : 4);
    ui.sbOPerm->setValue((u <= 7) ? u : 4);

    encodingMethodChanged((int) GlobalConfig().ppEncodingMethod());
    showAllEncodingsClicked();

    ui.gbPpEnable->setChecked(GlobalConfig().ppEnabled());
}

void frmOptions::restoreDefaults()
{
    GlobalConfig().setP7zipPath("");
    GlobalConfig().setTmpPath(QDir::tempPath());
    GlobalConfig().setLanguage("en");
    GlobalConfig().setLanguageBackup("de");
    GlobalConfig().setNoBackup(false);
    GlobalConfig().setQuietBatch(false);
    GlobalConfig().setChangePermissions(false);
    GlobalConfig().setChangePermissionsTo("644");

#ifdef Q_OS_MAC
    GlobalConfig().setShowDockIcon(true);
#endif

    QList<QPair<QString, bool> > engines;
    engines << QPair<QString, bool>("NapiProjekt", true)
            << QPair<QString, bool>("OpenSubtitles", true)
            << QPair<QString, bool>("Napisy24", true);
    GlobalConfig().setEngines(engines);
    GlobalConfig().setSearchPolicy(SP_SEARCH_ALL);
    GlobalConfig().setDownloadPolicy(DP_SHOW_LIST_IF_NEEDED);

    GlobalConfig().setPpEnabled(false);
    GlobalConfig().setPpEncodingMethod(CEM_ORIGINAL);
    GlobalConfig().setPpAutoDetectEncoding(true);
    GlobalConfig().setPpEncodingFrom("windows-1250");
    GlobalConfig().setPpEncodingTo("UTF-8");
    GlobalConfig().setPpShowAllEncodings(false);
    GlobalConfig().setPpRemoveLines(false);
    QStringList words;
    words << "movie info" << "synchro";
    GlobalConfig().setPpRemoveWords(words);
    GlobalConfig().setPpSubFormat("");
    GlobalConfig().setPpSubExtension("");
    GlobalConfig().setPpSkipConvertAds(false);

    GlobalConfig().save();

    readConfig();
}





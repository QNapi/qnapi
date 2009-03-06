/*****************************************************************************
** QNapi
** Copyright (C) 2008-2009 Krzemin <pkrzemin@o2.pl>
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

frmOptions::frmOptions(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	if ( QSysInfo::MacintoshVersion == QSysInfo::MV_10_5) // bo na Leopardzie nie ma juz stylu BM
	{
		ui.cbUseBrushedMetal->setText(tr("Używaj ciemnego stylu (Mac OS X Leopard)"));
	}

	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#else
	ui.cbUseBrushedMetal->hide();
	ui.cbShowDockIcon->hide();
#endif

#ifdef Q_WS_WIN
	// Pod Windowsami chowamy kontrolki odpowiadajace za zmiane uprawnien - i tak sie nie przydadza
	ui.cbChangePermissions->hide();
	ui.sbOPerm->hide();
	ui.sbGPerm->hide();
	ui.sbUPerm->hide();
#endif

	QString tlcode;
	foreach(QString lang, QNapiLanguage("").listLanguages())
	{
		tlcode = QNapiLanguage(lang).toTwoLetter();
		ui.cbLang->addItem(QIcon(QString(":/languages/%1.gif").arg(tlcode)),
							lang,
							QVariant(tlcode));
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

	connect(ui.cbChangeEncoding, SIGNAL(clicked()), this, SLOT(changeEncodingClicked()));
	connect(ui.cbAutoDetectEncoding, SIGNAL(clicked()), this, SLOT(autoDetectEncodingClicked()));
	connect(ui.cbShowAllEncodings, SIGNAL(clicked()), this, SLOT(showAllEncodingsClicked()));
	connect(ui.cbUseBrushedMetal, SIGNAL(clicked()), this, SLOT(useBrushedMetalClicked()));

	connect(ui.pbRestoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

	showAllEncodings();

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2,
		(QApplication::desktop()->height() - height()) / 2);
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
	QString path7z = QFileDialog::getOpenFileName(this, tr("Wskaż ścieżkę do programu 7z"),
													QFileInfo(ui.le7zPath->text()).path());
	if(!path7z.isEmpty())
	{
		if(!QFileInfo(path7z).isExecutable())
			QMessageBox::warning(this, tr("Niepoprawna ścieżka"),
				tr("Wskazana przez Ciebie ścieżka do programu 7z jest niepoprawna. Jeśli nie możesz "
					"odnaleźć programu 7z, spróbuj zainstalować pakiet p7zip-full."));
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
									tr("Wskaż katalog tymczasowy"),
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

	QNapiAbstractEngine * e;
	e = n.engineByName(ui.twEngines->selectedItems().at(0)->text());

	int currentRow = ui.twEngines->row(ui.twEngines->selectedItems().at(0));

	ui.pbMoveUp->setEnabled(currentRow > 0);
	ui.pbMoveDown->setEnabled(currentRow < ui.twEngines->rowCount() - 1);
	ui.pbEngineConf->setEnabled(e->isConfigurable());
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
							"Ostrzeżenie",
							"Przynajmniej jeden moduł pobierania musi pozostać aktywny!");
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
	n.addEngines(n.enumerateEngines());
	n.engineByName(ui.twEngines->selectedItems().at(0)->text())->configure(this);
}

void frmOptions::pbEngineInfoClicked()
{
	QNapi n;
	n.addEngines(n.enumerateEngines());
	QString engineName = ui.twEngines->selectedItems().at(0)->text();
	QString engineInfo = n.engineByName(engineName)->engineInfo();
	
	QMessageBox::information(	this,
								QString("Informacje o silniku %1").arg(engineName),
								engineInfo);
}


void frmOptions::changeEncodingClicked()
{
	bool checked = ui.cbChangeEncoding->isChecked();
	ui.cbEncFrom->setEnabled(checked);
	ui.lbConvert->setEnabled(checked);
	ui.lbConvertFrom->setEnabled(checked);
	ui.cbEncTo->setEnabled(checked);
	ui.cbAutoDetectEncoding->setEnabled(checked);
	ui.cbShowAllEncodings->setEnabled(checked);
	autoDetectEncodingClicked();
}

void frmOptions::autoDetectEncodingClicked()
{
	bool checkedCE = ui.cbChangeEncoding->isChecked();
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

void frmOptions::useBrushedMetalClicked()
{
	setAttribute(Qt::WA_MacBrushedMetal, ui.cbUseBrushedMetal->isChecked());
}

void frmOptions::writeConfig()
{
	GlobalConfig().setP7zipPath(ui.le7zPath->text());
	GlobalConfig().setTmpPath(ui.leTmpPath->text());
	GlobalConfig().setLanguage(ui.cbLang->itemData(ui.cbLang->currentIndex()).toString());
	GlobalConfig().setNoBackup(ui.cbNoBackup->isChecked());

#ifdef Q_OS_MAC
	GlobalConfig().setShowDockIcon(ui.cbShowDockIcon->isChecked());
	GlobalConfig().setUseBrushedMetal(ui.cbUseBrushedMetal->isChecked());
#endif

	QList<QPair<QString, bool> > engines;
	for(int i = 0; i < ui.twEngines->rowCount(); ++i)
	{
		engines << qMakePair(ui.twEngines->item(i, 0)->text(),
							(ui.twEngines->item(i, 0)->checkState() == Qt::Checked));
	}

	GlobalConfig().setEngines(engines);
	
	GlobalConfig().setSearchPolicy((SearchPolicy)ui.cbSearchPolicy->currentIndex());
	GlobalConfig().setDownloadPolicy((DownloadPolicy)ui.cbDownloadPolicy->currentIndex());

	GlobalConfig().setPpEnabled(ui.gbPpEnable->isChecked());
	GlobalConfig().setPpChangeEncoding(ui.cbChangeEncoding->isChecked());
	GlobalConfig().setPpAutoDetectEncoding(ui.cbAutoDetectEncoding->isChecked());
	GlobalConfig().setPpEncodingFrom(ui.cbEncFrom->currentText());
	GlobalConfig().setPpEncodingTo(ui.cbEncTo->currentText());
	GlobalConfig().setPpShowAllEncodings(ui.cbShowAllEncodings->isChecked());
	GlobalConfig().setPpRemoveLines(ui.cbRemoveLines->isChecked());
	GlobalConfig().setPpRemoveWords(ui.teRemoveWords->toPlainText().split("\n"));
	GlobalConfig().setPpChangePermissions(ui.cbChangePermissions->isChecked());

	QString permissions = QString("%1%2%3").arg(ui.sbUPerm->value())
										   .arg(ui.sbGPerm->value())
										   .arg(ui.sbOPerm->value());
	GlobalConfig().setPpPermissions(permissions);

	GlobalConfig().save();
}

void frmOptions::readConfig()
{
	GlobalConfig().reload();

	ui.le7zPath->setText(GlobalConfig().p7zipPath());
	ui.leTmpPath->setText(GlobalConfig().tmpPath());
	ui.cbLang->setCurrentIndex(ui.cbLang->findData(QNapiLanguage(GlobalConfig().language()).toTwoLetter()));

	ui.cbNoBackup->setChecked(GlobalConfig().noBackup());

#ifdef Q_OS_MAC
	ui.cbShowDockIcon->setChecked(GlobalConfig().showDockIcon());
	ui.cbUseBrushedMetal->setChecked(GlobalConfig().useBrushedMetal());
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
		QTableWidgetItem *item = new QTableWidgetItem(n.engineByName(e.first)->engineIcon(), e.first);
		item->setCheckState(e.second ? Qt::Checked : Qt::Unchecked);
		ui.twEngines->setItem(i, 0, item);
	}

	ui.twEngines->horizontalHeader()->hide();
	ui.twEngines->verticalHeader()->hide();
	ui.twEngines->verticalHeader()->setDefaultSectionSize(20);
	ui.twEngines->verticalHeader()->setResizeMode(QHeaderView::Fixed);
	ui.twEngines->setColumnWidth(0, 300);


	ui.cbSearchPolicy->setCurrentIndex(GlobalConfig().searchPolicy());
	ui.cbDownloadPolicy->setCurrentIndex(GlobalConfig().downloadPolicy());

	ui.cbChangeEncoding->setChecked(GlobalConfig().ppChangeEncoding());
	ui.cbAutoDetectEncoding->setChecked(GlobalConfig().ppAutoDetectEncoding());
	ui.cbEncFrom->setCurrentIndex(ui.cbEncFrom->findText(GlobalConfig().ppEncodingFrom()));
	ui.cbEncTo->setCurrentIndex(ui.cbEncTo->findText(GlobalConfig().ppEncodingTo()));
	ui.cbShowAllEncodings->setChecked(GlobalConfig().ppShowAllEncodings());
	ui.cbRemoveLines->setChecked(GlobalConfig().ppRemoveLines());
	ui.teRemoveWords->setText(GlobalConfig().ppRemoveWords().join("\n"));
	ui.cbChangePermissions->setChecked(GlobalConfig().ppChangePermissions());

	QString permissions = GlobalConfig().ppPermissions();
	unsigned short o, g, u;
	o = permissions.at(0).toAscii() - '0';
	g = permissions.at(1).toAscii() - '0';
	u = permissions.at(2).toAscii() - '0';
	ui.sbUPerm->setValue((o <= 7) ? o : 6);
	ui.sbGPerm->setValue((g <= 7) ? g : 4);
	ui.sbOPerm->setValue((u <= 7) ? u : 4);

	changeEncodingClicked();
	showAllEncodingsClicked();

	ui.gbPpEnable->setChecked(GlobalConfig().ppEnabled());
}

void frmOptions::restoreDefaults()
{
	GlobalConfig().setP7zipPath("");
	GlobalConfig().setTmpPath(QDir::tempPath());
	GlobalConfig().setLanguage("pl");
	GlobalConfig().setNoBackup(false);

#ifdef Q_OS_MAC
	GlobalConfig().setShowDockIcon(true);
	GlobalConfig().setUseBrushedMetal(false);
#endif

	QList<QPair<QString, bool> > engines;
	engines << QPair<QString, bool>("NapiProjekt", true)
			<< QPair<QString, bool>("OpenSubtitles", true);
	GlobalConfig().setEngines(engines);
	GlobalConfig().setSearchPolicy(SP_SEARCH_ALL);
	GlobalConfig().setDownloadPolicy(DP_SHOW_LIST_IF_NEEDED);

	GlobalConfig().setPpEnabled(false);
	GlobalConfig().setPpChangeEncoding(false);
	GlobalConfig().setPpAutoDetectEncoding(false);
	GlobalConfig().setPpEncodingFrom("windows-1250");
	GlobalConfig().setPpEncodingTo("UTF-8");
	GlobalConfig().setPpShowAllEncodings(false);
	GlobalConfig().setPpRemoveLines(false);
	QStringList words;
	words << "movie info" << "synchro";
	GlobalConfig().setPpRemoveWords(words);
	GlobalConfig().setPpChangePermissions(false);
	GlobalConfig().setPpPermissions("644");

	GlobalConfig().save();

	readConfig();
}





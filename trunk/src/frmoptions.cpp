/*****************************************************************************
** QNapi
** Copyright (C) 2008 Krzemin <pkrzemin@o2.pl>
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
		ui.cbUseBrushedMetal->setText(tr("Używaj przyciemnionych okien (Mac OS X Leopard)"));
	}

	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#else
	ui.cbUseBrushedMetal->hide();
#endif

#ifdef Q_WS_WIN
	// Pod Windowsami chowamy kontrolki odpowiadajace za zmiane uprawnien - i tak sie nie przydadza
	ui.cbChangePermissions->hide();
	ui.sbOPerm->hide();
	ui.sbGPerm->hide();
	ui.sbUPerm->hide();
#endif

	setAttribute(Qt::WA_QuitOnClose, false);

	connect(ui.le7zPath, SIGNAL(textChanged(const QString &)), this, SLOT(le7zPathChanged()));
	connect(ui.pb7zPathSelect, SIGNAL(clicked()), this, SLOT(select7zPath()));
	connect(ui.leTmpPath, SIGNAL(textChanged(const QString &)), this, SLOT(leTmpPathChanged()));
	connect(ui.pbTmpPathSelect, SIGNAL(clicked()), this, SLOT(selectTmpPath()));
//	connect(ui.pbRegister, SIGNAL(clicked()), this, SLOT(pbRegisterClicked()));
	connect(ui.cbChangeEncoding, SIGNAL(clicked()), this, SLOT(changeEncodingClicked()));
	connect(ui.cbAutoDetectEncoding, SIGNAL(clicked()), this, SLOT(autoDetectEncodingClicked()));
	connect(ui.cbShowAllEncodings, SIGNAL(clicked()), this, SLOT(showAllEncodingsClicked()));
	connect(ui.cbUseBrushedMetal, SIGNAL(clicked()), this, SLOT(useBrushedMetalClicked()));

	showAllEncodings();


//wypelnianie tvEngines

//	ui.tvEngines->setItemDelegateForColumn(1, col2Delegate);
///	ui.tvEngines->setItemDelegate(new QNapiEngineConfigItemDelegate(this));
	
	ui.tvEngines->setModel(&enginesModel);
	ui.tvEngines->verticalHeader()->hide();
	ui.tvEngines->verticalHeader()->setDefaultSectionSize(20);
	ui.tvEngines->verticalHeader()->setResizeMode(QHeaderView::Fixed);
	ui.tvEngines->setColumnWidth(0, 300);



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
									QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
	if(!tmpDir.isEmpty())
		ui.leTmpPath->setText(QFileInfo(tmpDir).path());
}

void frmOptions::pbRegisterClicked()
{
	((QNapiApp*)qApp)->showCreateUser();
	readConfig();
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
//	GlobalConfig().setNick(ui.leNick->text());
//	GlobalConfig().setPass(ui.lePass->text());
	GlobalConfig().setLanguage((ui.cbLang->currentIndex() == 0) ? "PL" : "ENG");
	GlobalConfig().setNoBackup(ui.cbNoBackup->isChecked());
	GlobalConfig().setUseBrushedMetal(ui.cbUseBrushedMetal->isChecked());

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
	//ui.leNick->setText(GlobalConfig().nick());
	//ui.lePass->setText(GlobalConfig().pass());
	ui.cbLang->setCurrentIndex((GlobalConfig().language() == "PL") ? 0 : 1);
	ui.cbNoBackup->setChecked(GlobalConfig().noBackup());
	ui.cbUseBrushedMetal->setChecked(GlobalConfig().useBrushedMetal());

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

/* QNapi
 * Autor: Krzemin <pkrzemin@o2.pl>
 * Ten program jest rozprowadzany na licencji GNU General Public License.
 */

#include "frmoptions.h"

frmOptions::frmOptions(QWidget * parent, Qt::WFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

#ifdef Q_WS_MAC
	if ( QSysInfo::MacintoshVersion == QSysInfo::MV_10_5) //bo na Leopardzie nie ma juz stylu BM
	{
		ui.cbUseBrushedMetal->setText(tr("Używaj przyciemnionych okien (Mac OS X Leopard)"));
	}

	setAttribute(Qt::WA_MacBrushedMetal, GlobalConfig().useBrushedMetal());
#else
	ui.cbUseBrushedMetal->setVisible(false);
#endif

	setAttribute(Qt::WA_QuitOnClose, false);

	connect(ui.le7zPath, SIGNAL(textChanged(const QString &)), this, SLOT(le7zPathChanged()));
	connect(ui.pb7zPathSelect, SIGNAL(clicked()), this, SLOT(select7zPath()));
	connect(ui.leTmpPath, SIGNAL(textChanged(const QString &)), this, SLOT(leTmpPathChanged()));
	connect(ui.pbTmpPathSelect, SIGNAL(clicked()), this, SLOT(selectTmpPath()));
	connect(ui.cbChangeEncoding, SIGNAL(clicked()), this, SLOT(changeEncodingClicked()));
	connect(ui.cbAutoDetectEncoding, SIGNAL(clicked()), this, SLOT(autoDetectEncodingClicked()));
	connect(ui.cbShowAllEncodings, SIGNAL(clicked()), this, SLOT(showAllEncodingsClicked()));
	connect(ui.cbUseBrushedMetal, SIGNAL(clicked()), this, SLOT(useBrushedMetalClicked()));

	showAllEncodings();

	// workaround dla compiza?
	move((QApplication::desktop()->width() - width()) / 2,
		(QApplication::desktop()->height() - height()) / 2);
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
		if(!QFileInfo(path7z).isExecutable())
			QMessageBox::warning(this, tr("Niepoprawna ścieżka"),
				tr("Wskazana przez Ciebie ścieżka do programu 7z jest niepoprawna. Jeśli nie możesz "
					"odnaleźć programu 7z, spróbuj zainstalować pakiet p7zip-full."));
		else
			ui.le7zPath->setText(path7z);
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
	GlobalConfig().setNick(ui.leNick->text());
	GlobalConfig().setPass(ui.lePass->text());
	GlobalConfig().setLanguage((ui.cbLang->currentIndex() == 0) ? "PL" : "ENG");
	GlobalConfig().setNoBackup(ui.cbNoBackup->isChecked());
	GlobalConfig().setChangeEncoding(ui.cbChangeEncoding->isChecked());
	GlobalConfig().setAutoDetectEncoding(ui.cbAutoDetectEncoding->isChecked());
	GlobalConfig().setEncodingFrom(ui.cbEncFrom->currentText());
	GlobalConfig().setEncodingTo(ui.cbEncTo->currentText());
	GlobalConfig().setShowAllEncodings(ui.cbShowAllEncodings->isChecked());
	GlobalConfig().setUseBrushedMetal(ui.cbUseBrushedMetal->isChecked());
	GlobalConfig().save();
}

void frmOptions::readConfig()
{
	GlobalConfig().reload();
	ui.le7zPath->setText(GlobalConfig().p7zipPath());
	ui.leTmpPath->setText(GlobalConfig().tmpPath());
	ui.leNick->setText(GlobalConfig().nick());
	ui.lePass->setText(GlobalConfig().pass());
	ui.cbLang->setCurrentIndex((GlobalConfig().language() == "PL") ? 0 : 1);
	ui.cbNoBackup->setChecked(GlobalConfig().noBackup());
	ui.cbChangeEncoding->setChecked(GlobalConfig().changeEncoding());
	ui.cbAutoDetectEncoding->setChecked(GlobalConfig().autoDetectEncoding());
	ui.cbEncFrom->setCurrentIndex(ui.cbEncFrom->findText(GlobalConfig().encodingFrom()));
	ui.cbEncTo->setCurrentIndex(ui.cbEncTo->findText(GlobalConfig().encodingTo()));
	ui.cbShowAllEncodings->setChecked(GlobalConfig().showAllEncodings());
	ui.cbUseBrushedMetal->setChecked(GlobalConfig().useBrushedMetal());
	changeEncodingClicked();
	showAllEncodingsClicked();
}

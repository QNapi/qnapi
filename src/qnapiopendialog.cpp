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

#include "qnapiopendialog.h"

QNapiOpenDialog::QNapiOpenDialog( QWidget * parent, const QString & caption,
									const QString & init_path,
									FilterMode filterMode )
							: QFileDialog(parent)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setWindowTitle(caption);

	if(filterMode == Movies)
	{
#if QT_VERSION >= 0x040400
		setNameFilter
#else
		setFilter
#endif
				(tr("Filmy (*.avi *.asf *.divx *.mkv *.mov *.mp4 *.mpeg *.mpg *.ogm "
					"*.rm *.rmvb *.wmv);;Wszystkie pliki (*.*)"));
	}
	else if(filterMode == Subtitles)
	{
#if QT_VERSION >= 0x040400
		setNameFilter
#else
		setFilter
#endif
				(tr("Napisy (*.txt);;Wszystkie pliki (*.*)"));
	}

	if(QFileInfo(init_path).isDir())
		setDirectory(init_path);
	else
		setDirectory(QDir::currentPath());

	QStringList sideUrls;
	
#ifdef Q_WS_MAC
	sideUrls << "/Volumes";
#endif

	sideUrls << QString(QDir::homePath() + "/Movies") << QString(QDir::homePath() + "/movies")
				<< QString(QDir::homePath() + "/Video") << QString(QDir::homePath() + "/video")
				<< QString(QDir::homePath() + "/Filmy") << QString(QDir::homePath() + "/filmy")
				<< QString(QDir::homePath() + "/Wideo") << QString(QDir::homePath() + "/wideo");

	QList<QUrl> urls = sidebarUrls();

	foreach(QString sideUrl, sideUrls)
	{
		if(!QDir().exists(sideUrl)) continue;
		QUrl url = QUrl::fromLocalFile(sideUrl);
		if(!urls.contains(url))
			urls << url;
	}

	setSidebarUrls(urls);
}

bool QNapiOpenDialog::selectFile()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
	files.clear();
	QString file = getOpenFileName(this, windowTitle(), directory().path(),
#if QT_VERSION >= 0x040400
		nameFilters().join("\n")
#else
		filters().join("\n")
#endif
					);

	if(!file.isEmpty())
		files << file;

	return !file.isEmpty();
#else
	if(!placeWindow()) return false;
	setFileMode(QFileDialog::ExistingFile);
	return exec();
#endif
}

bool QNapiOpenDialog::selectFiles()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
	files = getOpenFileNames(this, windowTitle(), directory().path(),
#if QT_VERSION >= 0x040400
		nameFilters().join("\n")
#else
		filters().join("\n")
#endif
					);

	return !files.isEmpty();	
#else
	if(!placeWindow()) return false;
	setFileMode(QFileDialog::ExistingFiles);
	return exec();
#endif
}

bool QNapiOpenDialog::selectDirectory()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
	files.clear();
	QString dir = getExistingDirectory(this, windowTitle(), directory().path());

	if(dir == directory().path())
		dir = "";

	if(!dir.isEmpty())
		files << dir;

	return !dir.isEmpty();
#else
	if(!placeWindow()) return false;
	// QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks ?
	setFileMode(QFileDialog::DirectoryOnly);
	return exec();
#endif
}

#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
QStringList QNapiOpenDialog::selectedFiles() const
{
	return files;
}
#endif

bool QNapiOpenDialog::placeWindow()
{
	if(isVisible())
	{
		raise();
		return false;
	}

	// workaround dla compiza
	move((QApplication::desktop()->width() - width()) / 2, 
		(QApplication::desktop()->height() - height()) / 2);

	return true;
}

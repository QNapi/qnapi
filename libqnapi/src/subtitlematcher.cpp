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

#include "subtitlematcher.h"
#include <QDir>

SubtitleMatcher::SubtitleMatcher(
    bool _noBackup, bool _isPostProcessingEnabled, QString _ppSubFormat,
    QString _ppSubExtension, bool _changePermissions,
    QString _changePermissionsTo,
    const QSharedPointer<const SubtitleFormatsRegistry>&
        subtitleFormatsRegistry)
    : noBackup(_noBackup),
      isPostProcessingEnabled(_isPostProcessingEnabled),
      ppSubFormat(_ppSubFormat),
      ppSubExtension(_ppSubExtension),
      changePermissions(_changePermissions),
      changePermissionsTo(_changePermissionsTo),
      subtitleFormatsRegistry(subtitleFormatsRegistry) {}

bool SubtitleMatcher::matchSubtitles(QString subtitlesTmpFilePath,
                                     QString targetMovieFilePath) const {
  QFileInfo subtitlesTmpFileInfo(subtitlesTmpFilePath);

  if (!subtitlesTmpFileInfo.exists()) return false;

  QString targetExtension = selectTargetExtension(subtitlesTmpFileInfo);

  QString targetSubtitlesFilePath =
      constructSubtitlePath(targetMovieFilePath, targetExtension);

  if (!isWritablePath(targetSubtitlesFilePath)) return false;

  removeOrCopy(targetMovieFilePath, targetSubtitlesFilePath);

  bool result = false;

#ifdef Q_OS_WIN
  // Pod windowsem, aby "wyczyscic" atrybuty pliku, tworzymy go na nowo
  result = dryCopy(subtitlesTmpFilePath, targetSubtitlesFilePath);
#else
  // pod innymi OS-ami wystarczy skopiowac plik
  result = QFile::copy(subtitlesTmpFilePath, targetSubtitlesFilePath);
#endif

  if (changePermissions) {
    fixFilePermissions(targetSubtitlesFilePath, changePermissionsTo);
  }

  return result;
}

QString SubtitleMatcher::selectTargetExtension(
    QFileInfo subtitlesTmpFileInfo) const {
  QString targetExtension = subtitlesTmpFileInfo.suffix();

  if (isPostProcessingEnabled) {
    if (!ppSubFormat.isEmpty() && ppSubExtension.isEmpty()) {
      targetExtension =
          subtitleFormatsRegistry->select(ppSubFormat)->defaultExtension();
    } else if (!ppSubExtension.isEmpty()) {
      targetExtension = ppSubExtension;
    }
  }

  return targetExtension;
}

QString SubtitleMatcher::constructSubtitlePath(QString targetMovieFilePath,
                                               QString targetExtension,
                                               QString baseSuffix) const {
  QFileInfo targetMovieFileInfo(targetMovieFilePath);
  return targetMovieFileInfo.path() + QDir::separator() +
         targetMovieFileInfo.completeBaseName() + baseSuffix + "." +
         targetExtension;
}

bool SubtitleMatcher::isWritablePath(QString path) const {
  QFileInfo pathFileInfo(path);
  return QFileInfo(pathFileInfo.absolutePath()).isWritable();
}

void SubtitleMatcher::removeOrCopy(QString targetMoviefilePath,
                                   QString targetSubtitlesFilePath) const {
  if (QFile::exists(targetSubtitlesFilePath)) {
    if (!noBackup) {
      QFileInfo targetSubtitlesFileInfo(targetSubtitlesFilePath);
      QString newName = constructSubtitlePath(
          targetMoviefilePath, targetSubtitlesFileInfo.suffix(), tr("_copy"));

      if (QFile::exists(newName)) QFile::remove(newName);

      QFile::rename(targetSubtitlesFilePath, newName);
    } else
      QFile::remove(targetSubtitlesFilePath);
  }
}

bool SubtitleMatcher::dryCopy(QString srcFilePath, QString dstFilePath) const {
  QFile dstFile(dstFilePath), srcFile(srcFilePath);
  bool result = false;

  if (!dstFile.open(QIODevice::WriteOnly) ||
      !srcFile.open(QIODevice::ReadOnly)) {
    dstFile.close();
  } else {
    result = dstFile.write(srcFile.readAll()) > 0;
    srcFile.close();
    dstFile.close();
  }

  return result;
}

void SubtitleMatcher::fixFilePermissions(QString targetSubtitlesFilePath,
                                         QString changePermissionsTo) const {
  bool validPermissions;
  int permInt = changePermissionsTo.toInt(&validPermissions, 8);

  if (validPermissions) {
    int perm = 0;
    perm |= (permInt & 0700) << 2;
    perm |= (permInt & 0070) << 1;
    perm |= (permInt & 0007);
    QFile::setPermissions(targetSubtitlesFilePath, QFile::Permissions(perm));
  }
}

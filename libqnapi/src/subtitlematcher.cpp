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
    bool _noBackup, LangCodeType _langCodeInFileName,
    bool _isPostProcessingEnabled, QString _ppSubFormat,
    QString _ppSubExtension, bool _changePermissions,
    QString _changePermissionsTo,
    const QSharedPointer<const SubtitleFormatsRegistry>&
        subtitleFormatsRegistry)
    : noBackup(_noBackup),
      langCodeInFileName(_langCodeInFileName),
      isPostProcessingEnabled(_isPostProcessingEnabled),
      ppSubFormat(_ppSubFormat),
      ppSubExtension(_ppSubExtension),
      changePermissions(_changePermissions),
      changePermissionsTo(_changePermissionsTo),
      subtitleFormatsRegistry(subtitleFormatsRegistry) {}

/**
 * Copy temporary subtitles file into rightful place.
 *
 * Given temporary file will be placed next to the given video file
 * and renamed to match video file name. Optional features will be
 * applied accordingly to given settings.
 *
 * @param subtitlesTmpFilePath Path to the temporary file.
 * @param targetMovieFilePath Path to the video file.
 * @param subtitlesLanguage Language of the subtitles. Used only when "language
 *                          code in file name" option is different then
 *                          #LCT_NONE.
 * @return Success or failure.
 */
bool SubtitleMatcher::matchSubtitles(QString subtitlesTmpFilePath,
                                     QString targetMovieFilePath,
                                     QString subtitlesLanguage) const {
  QFileInfo subtitlesTmpFileInfo(subtitlesTmpFilePath);

  if (!subtitlesTmpFileInfo.exists()) return false;

  QString targetExtension = selectTargetExtension(subtitlesTmpFileInfo);

  QString targetSubtitlesFilePath =
      constructSubtitlePath(targetMovieFilePath, targetExtension,
      subtitlesLanguage);

  if (!isWritablePath(targetSubtitlesFilePath)) return false;

  removeOrCopy(targetMovieFilePath, targetSubtitlesFilePath, subtitlesLanguage);

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
                                               QString subtitlesLanguage,
                                               QString baseSuffix) const {
  QFileInfo targetMovieFileInfo(targetMovieFilePath);
  QString ret = targetMovieFileInfo.path() + QDir::separator() +
         targetMovieFileInfo.completeBaseName() + baseSuffix + ".";

  if (langCodeInFileName != LCT_NONE) {
    QString langCode = SubtitleLanguage(subtitlesLanguage).toString(
        langCodeInFileName);
    if (!langCode.isEmpty()) {
      ret += langCode;
      ret += ".";
    }
  }

  ret += targetExtension;
  return ret;
}

bool SubtitleMatcher::isWritablePath(QString path) const {
  QFileInfo pathFileInfo(path);
  return QFileInfo(pathFileInfo.absolutePath()).isWritable();
}

void SubtitleMatcher::removeOrCopy(QString targetMoviefilePath,
                                   QString targetSubtitlesFilePath,
                                   QString subtitlesLanguage) const {
  if (QFile::exists(targetSubtitlesFilePath)) {
    if (!noBackup) {
      QFileInfo targetSubtitlesFileInfo(targetSubtitlesFilePath);
      QString newName = constructSubtitlePath(
          targetMoviefilePath, targetSubtitlesFileInfo.suffix(),
          subtitlesLanguage, tr("_copy"));

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

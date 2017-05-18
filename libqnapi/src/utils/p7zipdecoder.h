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

#ifndef P7ZIPDECODER_H
#define P7ZIPDECODER_H

#include <QString>
#include <QStringList>

class P7ZipDecoder {
 public:
  P7ZipDecoder(const QString& p7zipPath, int operationTimeoutMsecs = 10000);

  QStringList listArchiveFiles(const QString& archivePath) const;

  bool unpackArchiveFile(const QString& archivePath, const QString& filename,
                         const QString& targetPath) const;
  bool unpackArchiveFiles(const QString& archivePath,
                          const QString& targetPath) const;
  bool unpackSecureArchiveFiles(const QString& archivePath,
                                const QString& password,
                                const QString& targetPath) const;

 private:
  const QString p7zipPath;
  int operationTimeoutMsecs;

  bool runP7Zip(const QStringList& args) const;
  QString readP7ZipOutput(const QStringList& args) const;
};

#endif  // P7ZIPDECODER_H

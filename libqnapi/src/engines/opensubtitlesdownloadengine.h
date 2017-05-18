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

#ifndef OPENSUBTITLESDOWNLOADENGINE_H
#define OPENSUBTITLESDOWNLOADENGINE_H

#include "config/engineconfig.h"
#include "engines/subtitledownloadengine.h"
#include "utils/p7zipdecoder.h"
#include "utils/syncxmlrpc.h"

class OpenSubtitlesDownloadEngine : public SubtitleDownloadEngine {
 public:
  OpenSubtitlesDownloadEngine(
      const QString& tmpPath, const EngineConfig& config,
      const QSharedPointer<const P7ZipDecoder>& p7zipDecoder,
      const QString& qnapiDisplayableVersion, const QString& language);
  ~OpenSubtitlesDownloadEngine();

  static SubtitleDownloadEngineMetadata metadata;
  static const char* const pixmapData[];

  SubtitleDownloadEngineMetadata meta() const;
  const char* const* enginePixmapData() const;

  QString checksum(QString filename = "");
  bool lookForSubtitles(QString lang);
  QList<SubtitleInfo> listSubtitles();
  bool download(QUuid id);
  bool unpack(QUuid id);
  void cleanup();

 private:
  EngineConfig engineConfig;
  QSharedPointer<const P7ZipDecoder> p7zipDecoder;
  QString qnapiDisplayableVersion;
  QString language;
  SyncXmlRpc rpc;

  quint64 fileSize;
  QString subFileName, token;

  // sprawdza czy dana instancja klasy jest zalogowana na sewerze
  bool isLogged() { return !token.isEmpty(); }
  // loguje na serwer OpenSubtitles
  bool login();
  // wylogowuje z serwera
  void logout();
};

#endif

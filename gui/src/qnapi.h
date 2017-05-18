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

#ifndef __QNAPI__H__
#define __QNAPI__H__

#include "config/qnapiconfig.h"
#include "engines/subtitledownloadengine.h"
#include "engines/subtitledownloadenginesregistry.h"
#include "subtitleinfo.h"

#include <Maybe.h>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

class QNapi {
 public:
  QNapi(const QNapiConfig& config,
        const Maybe<QString>& specificEngine = nothing());
  ~QNapi();

  bool checkP7ZipPath();
  bool checkTmpPath();
  bool ppEnabled();

  void setMoviePath(QString path);
  QString moviePath();
  bool checkWritePermissions();

  void clearSubtitlesList();
  void checksum();
  bool lookForSubtitles(QString lang, QString engine = "");
  bool lookForSubtitles(QStringList languages, QString engine = "");
  QList<SubtitleInfo> listSubtitles();

  bool needToShowList();
  int bestIdx();

  bool download(int i);
  bool unpack(int i);
  bool matchSubtitles();
  void postProcessSubtitles() const;

  void cleanup();
  QString error();

  QStringList listLoadedEngines() const;

 private:
  QSharedPointer<SubtitleDownloadEngine> engineByName(QString name) const;

  QString movie;
  QString errorMsg;
  QList<QSharedPointer<SubtitleDownloadEngine>> enginesList;
  QList<SubtitleInfo> subtitlesList;
  QSharedPointer<SubtitleDownloadEngine> currentEngine;

  // najlepszy indeks napisow
  int theBestIdx;

  const QSharedPointer<const SubtitleDownloadEnginesRegistry> enginesRegistry;
  const QNapiConfig config;
};

#endif  // __QNAPI__H__

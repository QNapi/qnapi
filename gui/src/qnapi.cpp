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

#include "qnapi.h"
#include "libqnapi.h"
#include "subtitlematcher.h"
#include "subtitlepostprocessor.h"

#include "config/configreader.h"

#include <QtAlgorithms>

QNapi::QNapi(const QNapiConfig& config, const Maybe<QString>& specificEngine)
    : enginesRegistry(LibQNapi::subtitleDownloadEngineRegistry()),
      config(config) {
  if (specificEngine) {
    enginesList << enginesRegistry->createEngine(specificEngine.value(),
                                                 config);
  } else {
    enginesList << enginesRegistry->createEnabledEngines(config);
  }
}

QNapi::~QNapi() { cleanup(); }

bool QNapi::checkP7ZipPath() {
  return QFileInfo(config.generalConfig().p7zipPath()).isExecutable();
}

bool QNapi::checkTmpPath() {
  QFileInfo f(config.generalConfig().tmpPath());
  return f.isDir() && f.isWritable();
}

bool QNapi::ppEnabled() { return config.postProcessingConfig().enabled(); }

void QNapi::setMoviePath(QString path) {
  movie = path;
  currentEngine = QSharedPointer<SubtitleDownloadEngine>();
}

QString QNapi::moviePath() { return movie; }

bool QNapi::checkWritePermissions() {
  return QFileInfo(QFileInfo(movie).path()).isWritable();
}

void QNapi::clearSubtitlesList() {
  subtitlesList.clear();
  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    e->clearSubtitlesList();
  }
}

void QNapi::checksum() {
  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    e->checksum(movie);
  }
}

bool QNapi::lookForSubtitles(QString lang, QString engine) {
  bool result = false;

  if (engine.isEmpty()) {
    foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
      e->setMoviePath(movie);
      result = e->lookForSubtitles(lang) || result;
    }
  } else {
    QSharedPointer<SubtitleDownloadEngine> e = engineByName(engine);
    if (e) {
      e->setMoviePath(movie);
      result = e->lookForSubtitles(lang);
    }
  }

  if (!result) {
    errorMsg = QObject::tr("No subtitles found!");
  }

  return result;
}

QList<SubtitleInfo> QNapi::listSubtitles() {
  subtitlesList.clear();

  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    QList<SubtitleInfo> list = e->listSubtitles();
    subtitlesList << list;
  }

  QString mainLang = config.generalConfig().language();
  QString backupLang = config.generalConfig().backupLanguage();

  auto langRank = [&](QString lang) {
    if (lang == mainLang) return 0;
    if (lang == backupLang) return 1;
    return 2;
  };

  qStableSort(subtitlesList.begin(), subtitlesList.end(),
              [&](const SubtitleInfo& si1, const SubtitleInfo& si2) {
                return langRank(si1.lang) < langRank(si2.lang);
              });

  return subtitlesList;
}

bool QNapi::needToShowList() {
  theBestIdx = 0;

  int i = 0;
  bool foundBestIdx = false;
  foreach (SubtitleInfo s, listSubtitles()) {
    if (s.resolution == SUBTITLE_GOOD) {
      theBestIdx = i;
      foundBestIdx = true;
      break;
    }
    ++i;
  }

  if (config.generalConfig().downloadPolicy() == DP_ALWAYS_SHOW_LIST)
    return true;
  if (config.generalConfig().downloadPolicy() == DP_NEVER_SHOW_LIST)
    return false;

  if (listSubtitles().size() <= 1) return false;

  return !foundBestIdx;
}

int QNapi::bestIdx() { return theBestIdx; }

bool QNapi::download(int i) {
  SubtitleInfo s = subtitlesList[i];
  currentEngine = engineByName(s.engine);
  if (!currentEngine) return false;
  return currentEngine->download(s.id);
}

bool QNapi::unpack(int i) {
  return currentEngine ? currentEngine->unpack(subtitlesList[i].id) : false;
}

bool QNapi::matchSubtitles() {
  if (currentEngine) {
    QSharedPointer<const SubtitleMatcher> matcher =
        LibQNapi::subtitleMatcher(config);
    return matcher->matchSubtitles(currentEngine->subtitlesTmp,
                                   currentEngine->movie);
  }

  return false;
}

void QNapi::postProcessSubtitles() const {
  if (currentEngine) {
    QSharedPointer<const SubtitlePostProcessor> postProcessor =
        LibQNapi::subtitlePostProcessor(config.postProcessingConfig());

    postProcessor->perform(currentEngine->movie, currentEngine->subtitlesTmp);
  }
}

void QNapi::cleanup() {
  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    e->cleanup();
  }
}

QString QNapi::error() { return errorMsg; }

QStringList QNapi::listLoadedEngines() const {
  QStringList list;
  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    list << e->meta().name();
  }
  return list;
}

QSharedPointer<SubtitleDownloadEngine> QNapi::engineByName(QString name) const {
  foreach (QSharedPointer<SubtitleDownloadEngine> e, enginesList) {
    if (name == e->meta().name()) {
      return e;
    }
  }

  return QSharedPointer<SubtitleDownloadEngine>();
}

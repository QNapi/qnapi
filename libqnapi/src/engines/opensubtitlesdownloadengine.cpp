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

#include "opensubtitlesdownloadengine.h"
#include "subtitlelanguage.h"

#include <QDir>

namespace OpenSubtitlesDownloadEngineConst {
const QString openSubtitlesXmlRpcUrl = "http://api.opensubtitles.org/xml-rpc";
}

SubtitleDownloadEngineMetadata OpenSubtitlesDownloadEngine::metadata =
    SubtitleDownloadEngineMetadata(
        "OpenSubtitles",
        QObject::tr("<b>www.opensubtitles.org</b> subtitles download engine"),
        just(QUrl("http://www.opensubtitles.org/newuser")),
        just(QUrl("http://www.opensubtitles.org/upload")));

OpenSubtitlesDownloadEngine::OpenSubtitlesDownloadEngine(
    const QString& tmpPath, const EngineConfig& config,
    const QSharedPointer<const P7ZipDecoder>& p7zipDecoder,
    const QString& qnapiDisplayableVersion, const QString& language)
    : SubtitleDownloadEngine(tmpPath),
      engineConfig(config),
      p7zipDecoder(p7zipDecoder),
      qnapiDisplayableVersion(qnapiDisplayableVersion),
      language(language),
      rpc(QUrl(OpenSubtitlesDownloadEngineConst::openSubtitlesXmlRpcUrl)) {}

OpenSubtitlesDownloadEngine::~OpenSubtitlesDownloadEngine() {
  cleanup();
  if (isLogged()) logout();
}

SubtitleDownloadEngineMetadata OpenSubtitlesDownloadEngine::meta() const {
  return OpenSubtitlesDownloadEngine::metadata;
}

const char* const* OpenSubtitlesDownloadEngine::enginePixmapData() const {
  return OpenSubtitlesDownloadEngine::pixmapData;
}

// oblicza sume kontrolna dla pliku filmowego
QString OpenSubtitlesDownloadEngine::checksum(QString filename) {
  if (filename.isEmpty()) filename = movie;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) return QString("");

  fileSize = file.size();
  quint64 hash = fileSize;
  quint64 tmp, i;

  for (tmp = 0, i = 0;
       i < 65536 / sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp));
       i++, hash += tmp)
    ;
  file.seek(qMax(0, (int)((qint64)fileSize - 65536)));
  for (tmp = 0, i = 0;
       i < 65536 / sizeof(tmp) && file.read((char*)&tmp, sizeof(tmp));
       i++, hash += tmp)
    ;

  return (checkSum = QString("%1").arg(hash, 16, 16, QChar('0')));
}

// szuka napisow
bool OpenSubtitlesDownloadEngine::lookForSubtitles(QString lang) {
  if (checkSum.isEmpty()) return false;
  if (!isLogged() && !login()) return false;

  QVariantMap paramsMap;
  paramsMap["sublanguageid"] = SubtitleLanguage(lang).toTriLetter();
  paramsMap["moviehash"] = checkSum;
  paramsMap["moviebytesize"] = (int)fileSize;

  QVariantList requestList;
  requestList << paramsMap;

  QVariantList args;
  args << token << (QVariant)requestList;

  if (!rpc.request("SearchSubtitles", args)) return false;

  QVariantMap responseMap = rpc.getResponse().toMap();

  if (!responseMap.contains("data")) return false;

  QVariantList dataList = responseMap["data"].toList();
  QVariantList::iterator i;

  i = dataList.begin();
  while (i != dataList.end()) {
    responseMap = (*(i++)).toMap();

    if ((checkSum != responseMap["MovieHash"]) &&
        (fileSize != responseMap["MovieByteSize"]))
      continue;

    SubtitleResolution r = SUBTITLE_UNKNOWN;

    if (responseMap["SubBad"].toString() != "0") {
      r = SUBTITLE_BAD;
    } else if (QFileInfo(movie).completeBaseName() ==
               QFileInfo(responseMap["SubFileName"].toString())
                   .completeBaseName()) {
      r = SUBTITLE_GOOD;
    } else if (QRegExp(QString("^%1").arg(
                           responseMap["MovieReleaseName"].toString()),
                       Qt::CaseInsensitive)
                   .exactMatch(QFileInfo(movie).completeBaseName())) {
      r = SUBTITLE_GOOD;
    }

    QString subtitleName = responseMap["MovieReleaseName"].toString();
    if (subtitleName.isEmpty())
      subtitleName = QFileInfo(movie).completeBaseName();

    subtitlesList << SubtitleInfo(
        responseMap["ISO639"].toString(), meta().name(),
        responseMap["IDSubtitleFile"].toString(), subtitleName.trimmed(),
        responseMap["SubAuthorComment"].toString(),
        QFileInfo(responseMap["SubFileName"].toString()).suffix(), r);
  }

  return (subtitlesList.size() > 0);
}

QList<SubtitleInfo> OpenSubtitlesDownloadEngine::listSubtitles() {
  std::sort(subtitlesList.begin(), subtitlesList.end());
  return subtitlesList;
}

bool OpenSubtitlesDownloadEngine::download(QUuid id) {
  Maybe<SubtitleInfo> ms = resolveById(id);

  if (!ms) return false;

  SubtitleInfo s = ms.value();

  QVariantList paramsList;
  QVariantList requestList;

  subFileName = generateTmpFileName() + "." + s.format;
  paramsList << s.sourceLocation;
  requestList << (QVariant)paramsList;

  QVariantList args;
  args << token << requestList;

  if (!rpc.request("DownloadSubtitles", args)) return false;

  QVariantMap responseMap = rpc.getResponse().toMap();

  if (!responseMap.contains("data")) return false;

  QVariantList responseList = responseMap["data"].toList();

  if (responseList.size() == 0) return false;

  QByteArray subtitleContent =
      QByteArray::fromBase64(responseList.at(0).toMap()["data"].toByteArray());

  s.sourceLocation = generateTmpPath();
  QFile file(s.sourceLocation);
  if (file.exists()) file.remove();
  if (!file.open(QIODevice::WriteOnly)) return false;

  int r = file.write(subtitleContent);
  file.close();

  updateSubtitleInfo(s);

  return (bool)r;
}

bool OpenSubtitlesDownloadEngine::unpack(QUuid id) {
  Maybe<SubtitleInfo> ms = resolveById(id);
  if (!ms) return false;

  if (!QFile::exists(movie)) return false;

  QString archivePath = ms.value().sourceLocation;

  if (!p7zipDecoder->unpackArchiveFiles(archivePath, tmpPath)) return false;

  QString unpackedTmp =
      tmpPath + QDir::separator() + QFileInfo(archivePath).completeBaseName();
  subtitlesTmp = tmpPath + QDir::separator() + subFileName;

  QFile::copy(unpackedTmp, subtitlesTmp);
  QFile::remove(unpackedTmp);

  return true;
}

void OpenSubtitlesDownloadEngine::cleanup() {
  clearSubtitlesList();
  if (QFile::exists(subtitlesTmp)) QFile::remove(subtitlesTmp);
}

bool OpenSubtitlesDownloadEngine::login() {
  QString userAgent = QString("QNapi v%1").arg(qnapiDisplayableVersion);
  QVariantList args;
  args << engineConfig.nick() << engineConfig.password() << language.toLower()
       << userAgent;
  if (!rpc.request("LogIn", args)) return false;
  token = (rpc.getResponse().toMap())["token"].toString();

  return !token.isEmpty();
}

void OpenSubtitlesDownloadEngine::logout() {
  QVariantList args;
  args << token;
  rpc.request("LogOut", args);
  token = "";
}

const char* const OpenSubtitlesDownloadEngine::pixmapData[] = {
    "16 16 14 1",       ". c #000000",      "h c #111111",
    "c c #222222",      "j c #333333",      "g c #444444",
    "l c #555555",      "e c #777777",      "k c #888888",
    "a c #999999",      "b c #aaaaaa",      "f c #cccccc",
    "i c #dddddd",      "d c #eeeeee",      "# c #ffffff",
    "................", ".##.##.##.##.##.", ".##.##.##.##.##.",
    "................", "................", "...a##b..cd##d..",
    "..ea..be.fg.hi..", "..ic..hi.ig.....", "..dh..hd.g##ij..",
    "..ea..ak.k..gi..", "...b##b..fd##l..", "................",
    "................", ".##.##.##.##.##.", ".##.##.##.##.##.",
    "................"};

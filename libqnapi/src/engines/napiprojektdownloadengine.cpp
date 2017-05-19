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

#include "napiprojektdownloadengine.h"
#include "subtitlelanguage.h"
#include "utils/synchttp.h"

#include <cmath>

#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QUrl>

#ifdef Q_OS_WIN  // for SetFileAttributes
#include <qt_windows.h>
#endif

// This sadly has to be added in order to build under VS >= 2005
#ifdef _MSC_VER
// from Visual Studio 2005 onwards
// https://msdn.microsoft.com/en-us/library/2ts7cx93%28v=vs.80%29.aspx
#include <stdio.h>
#define snprintf _snprintf_s
#endif

namespace NapiProjektDownloadEngineConsts {
const unsigned long NAPI_10MB = 10485760;

const QString napiZipPassword = "iBlm8NTigvru0Jr0";
const QString napiDownloadUrlTpl =
    "http://www.napiprojekt.pl/unit_napisy/dl.php"
    "?l=%1&f=%2&t=%3&v=other&kolejka=false&nick=%4&pass=%5&napios=%6";
const QString napiCheckUserUrlTpl =
    "http://www.napiprojekt.pl/users_check.php?nick=%1&pswd=%2";
const QString napiUploadUrlTpl =
    "http://www.napiprojekt.pl/unit_napisy/upload.php"
    "?m_length=%1&m_resolution=%2x%3&m_fps=%4&m_hash=%5&m_filesize=%6";
const QString napiUploadUrlSimpleTpl =
    "http://www.napiprojekt.pl/unit_napisy/upload.php"
    "?m_hash=%5&m_filesize=%6";
const QString napiReportBadUrlTpl =
    "http://www.napiprojekt.pl/unit_napisy/zlenapisyadd.php";
}  // namespace NapiProjektDownloadEngineConsts

using namespace NapiProjektDownloadEngineConsts;

SubtitleDownloadEngineMetadata NapiProjektDownloadEngine::metadata =
    SubtitleDownloadEngineMetadata(
        "NapiProjekt",
        QObject::tr("<b>www.napiprojekt.pl</b> subtitles download engine"),
        just(QUrl("http://www.napiprojekt.pl/rejestracja")), nothing());

NapiProjektDownloadEngine::NapiProjektDownloadEngine(
    const QString &tmpPath, const EngineConfig &config,
    const QSharedPointer<const P7ZipDecoder> &p7zipDecoder)
    : SubtitleDownloadEngine(tmpPath),
      engineConfig(config),
      p7zipDecoder(p7zipDecoder) {}

NapiProjektDownloadEngine::~NapiProjektDownloadEngine() { cleanup(); }

SubtitleDownloadEngineMetadata NapiProjektDownloadEngine::meta() const {
  return NapiProjektDownloadEngine::metadata;
}

const char *const *NapiProjektDownloadEngine::enginePixmapData() const {
  return NapiProjektDownloadEngine::pixmapData;
}

QString NapiProjektDownloadEngine::checksum(QString filename) {
  if (filename.isEmpty()) filename = movie;
  return (checkSum = checksum(filename, true));
}

bool NapiProjektDownloadEngine::lookForSubtitles(QString lang) {
  Maybe<QString> tmpPackedFileOpt = downloadByLangAndChecksum(lang, checkSum);

  if (!tmpPackedFileOpt) {
    return false;
  }

  QString tmpPackedFile = tmpPackedFileOpt.value();

  subtitlesList << SubtitleInfo(lang, meta().name(), tmpPackedFile,
                                QFileInfo(movie).completeBaseName(), "", "txt",
                                SUBTITLE_UNKNOWN);
  return true;
}

QList<SubtitleInfo> NapiProjektDownloadEngine::listSubtitles() {
  return subtitlesList;
}

bool NapiProjektDownloadEngine::download(QUuid id) {
  Maybe<SubtitleInfo> ms = resolveById(id);

  return ms && QFile::exists(ms.value().sourceLocation);
}

Maybe<QString> NapiProjektDownloadEngine::downloadByLangAndChecksum(
    QString lang, QString checksum) const {
  if (checksum.isEmpty()) return nothing();

  QString urlTxt = napiDownloadUrlTpl.arg(npLangWrapper(lang))
                       .arg(checksum)
                       .arg(npFDigest(checksum))
                       .arg(engineConfig.nick())
                       .arg(engineConfig.password())
                       .arg(napiOS());

  QUrl url(urlTxt);

  SyncHTTP http;
  QNetworkReply *reply = http.syncGet(QNetworkRequest(url));

  if (reply->error() != QNetworkReply::NoError) return nothing();

  QByteArray buffer = reply->readAll();

  if (buffer.indexOf("NPc") == 0) return nothing();

  QString tmpPackedFile = generateTmpPath();

  QFile file(tmpPackedFile);
  if (file.exists()) file.remove();
  if (!file.open(QIODevice::WriteOnly)) return nothing();

  long r = file.write(buffer);
  file.close();

  if (r < 0) return nothing();

  return just(tmpPackedFile);
}

bool NapiProjektDownloadEngine::unpack(QUuid id) {
  Maybe<SubtitleInfo> ms = resolveById(id);
  if (!ms) return false;

  if (!QFile::exists(movie)) return false;
  subtitlesTmp = tmpPath + "/" + checkSum + ".txt";

  if (QFile::exists(subtitlesTmp)) QFile::remove(subtitlesTmp);

  QString archivePath = ms.value().sourceLocation;

  bool unpacked = p7zipDecoder->unpackSecureArchiveFiles(
      archivePath, napiZipPassword, tmpPath);

  return unpacked && QFile::exists(subtitlesTmp);
}

void NapiProjektDownloadEngine::cleanup() {
  clearSubtitlesList();
  if (QFile::exists(subtitlesTmp)) QFile::remove(subtitlesTmp);
}

bool NapiProjektDownloadEngine::checkUser(const QString &nick,
                                          const QString &pass) {
  SyncHTTP http;
  QString urlTxt = napiCheckUserUrlTpl.arg(nick).arg(pass);

  QUrl url(urlTxt);
  QNetworkReply *reply = http.syncGet(QNetworkRequest(url));
  if (reply->error() != QNetworkReply::NoError) return false;

  QString buffer = reply->readAll();
  if (buffer.indexOf("ok") == 0) return true;
  return false;
}

QString NapiProjektDownloadEngine::checksum(QString filename, bool limit10M) {
  QFile file(filename);
  QByteArray fileArray;

  if (!file.open(QIODevice::ReadOnly)) return QString("");

  fileArray = limit10M ? file.read(NAPI_10MB) : file.readAll();
  file.close();

  QByteArray b = QCryptographicHash::hash(fileArray, QCryptographicHash::Md5);
  fileArray.clear();

  QString out;
  char next[3];

  for (int i = 0; i < 16; i++) {
    snprintf(next, 3, "%.2x", static_cast<unsigned char>(b[i]));
    out += next;
  }

  checkSum = out;
  return checkSum;
}

QString NapiProjektDownloadEngine::npFDigest(const QString &input) const {
  if (input.size() != 32) return "";

  int idx[] = {0xe, 0x3, 0x6, 0x8, 0x2}, mul[] = {2, 2, 5, 4, 3},
      add[] = {0x0, 0xd, 0x10, 0xb, 0x5}, a, m, i, t, v;

  QString b;
  char tmp[2] = {0, 0};

  for (int j = 0; j <= 4; j++) {
    a = add[j];
    m = mul[j];
    i = idx[j];

    tmp[0] = input[i].toLatin1();
    t = a + static_cast<int>(strtol(tmp, NULL, 16));
    v = static_cast<int>(strtol(input.mid(t, 2).toLocal8Bit(), NULL, 16));

    snprintf(tmp, 2, "%x", (v * m) % 0x10);
    b += tmp;
  }

  return b;
}

QString NapiProjektDownloadEngine::npLangWrapper(QString lang) const {
  lang = SubtitleLanguage(lang).toTwoLetter().toUpper();

  if (lang == "EN") lang = "ENG";

  return lang;
}

QString NapiProjektDownloadEngine::napiOS() const {
#if defined(Q_OS_WIN)
  return "Windows";
#elif defined(Q_OS_MAC)
  return "Mac OS X";
#else
  return "Linux/UNIX";
#endif
}

const char *const NapiProjektDownloadEngine::pixmapData[] = {
    "16 16 5 1",        "   c #FFFFFF",     ".  c #000080",
    "+  c #0052CC",     "@  c #005CE6",     "#  c #BFD9FF",
    "           ...  ", "         ... .  ", "       ... ...  ",
    "     ... ....   ", "   ... ....     ", " ... ....       ",
    ".. ....         ", "....+           ", " ...............",
    " . . . . . . . .", " ...............", " .@@####@####@@.",
    " .@@#@@#@#@@#@@.", " .@@#@@#@####@@.", " .@@#@@#@#@@@@@.",
    " ..............."};

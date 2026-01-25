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

#include "subtitlepostprocessor.h"

#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>

SubtitlePostProcessor::SubtitlePostProcessor(
    const PostProcessingConfig& ppConfig,
    const QSharedPointer<const SubtitleConverter>& subtitleConverter)
    : ppConfig(ppConfig), subtitleConverter(subtitleConverter) {}

void SubtitlePostProcessor::perform(const QString& movieFilePath,
                                    const QString& subtitleFilePath) const {
  if (ppConfig.removeLines()) {
    ppRemoveLinesContainingWords(subtitleFilePath, ppConfig.removeLinesWords());
  }

  switch (ppConfig.encodingChangeMethod()) {
    case ECM_REPLACE_DIACRITICS:
      ppReplaceDiacriticsWithASCII(subtitleFilePath);
      break;
    case ECM_CHANGE:
      if (!ppConfig.encodingAutoDetectFrom() ||
          !ppChangeSubtitlesEncoding(subtitleFilePath, ppConfig.encodingTo())) {
        ppChangeSubtitlesEncoding(subtitleFilePath, ppConfig.encodingFrom(),
                                  ppConfig.encodingTo());
      }
      break;
    case ECM_ORIGINAL:
      // Nie ruszaj pobranych napisów!
      break;
  }

  if (!ppConfig.subFormat().isEmpty()) {
    QString targetFormat = ppConfig.subFormat();
    subtitleConverter->convertSubtitles(subtitleFilePath, targetFormat,
                                        subtitleFilePath, movieFilePath);
  }
}

bool SubtitlePostProcessor::ppReplaceDiacriticsWithASCII(
    const QString& subtitleFilePath) const {
  if (!QFileInfo(subtitleFilePath).exists()) return false;

  QString from = encodingUtils.detectFileEncoding(subtitleFilePath);

  if (from.isEmpty()) return false;

  QFile f(subtitleFilePath);
  if (!f.open(QIODevice::ReadOnly)) return false;

  QByteArray fileContent = f.readAll();
  f.close();

  // Fix: Proper UTF-16 handling
  QTextCodec* fromCodec;
  if (from.startsWith("UTF-16", Qt::CaseInsensitive)) {
    fromCodec = QTextCodec::codecForName("UTF-16");
  } else {
    fromCodec = QTextCodec::codecForName(qPrintable(from));
  }
  
  if (!fromCodec) return false;

  QString contentStr = fromCodec->toUnicode(fileContent);

  fileContent.clear();
  fileContent.append(encodingUtils.replaceDiacriticsWithASCII(contentStr));

  if (!f.open(QIODevice::WriteOnly)) return false;

  f.write(fileContent);
  f.close();

  return true;
}

bool SubtitlePostProcessor::ppChangeSubtitlesEncoding(
    const QString& subtitleFilePath, const QString& from,
    const QString& to) const {
  QFile f(subtitleFilePath);
  if (!f.open(QIODevice::ReadOnly)) return false;

  QByteArray fileContent = f.readAll();
  f.close();

  // Fix: Proper UTF-16 handling
  QTextCodec* fromCodec;
  if (from.startsWith("UTF-16", Qt::CaseInsensitive)) {
    fromCodec = QTextCodec::codecForName("UTF-16");
  } else {
    fromCodec = QTextCodec::codecForName(qPrintable(from));
  }
  
  if (!fromCodec) return false;

  QString contentStr = fromCodec->toUnicode(fileContent);

  if (to.compare("UTF-8", Qt::CaseInsensitive) != 0) {
    fileContent = QTextCodec::codecForName(qPrintable(to))
                      ->fromUnicode(contentStr.constData(), contentStr.size());
  } else {
    fileContent.clear();
    fileContent.append(contentStr.toUtf8());
  }

  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

  f.write(fileContent);
  f.close();

  return true;
}

bool SubtitlePostProcessor::ppChangeSubtitlesEncoding(
    const QString& subtitleFilePath, const QString& to) const {
  if (!QFileInfo(subtitleFilePath).exists()) return false;

  QString from = encodingUtils.detectFileEncoding(subtitleFilePath);

  if (from.isEmpty()) return false;

  return ppChangeSubtitlesEncoding(subtitleFilePath, from, to);
}

bool SubtitlePostProcessor::ppRemoveLinesContainingWords(
    const QString& subtitleFilePath, QStringList wordList) const {
  if (!QFileInfo(subtitleFilePath).exists()) return false;

  wordList = wordList.filter(QRegExp("^(.+)$"));

  QString fromCodec = encodingUtils.detectFileEncoding(subtitleFilePath);

  if (fromCodec.isEmpty()) fromCodec = ppConfig.encodingFrom();

  // Fix: Read as QString instead of QByteArray to preserve encoding
  QFile f(subtitleFilePath);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

  QTextStream in(&f);
  if (fromCodec.startsWith("UTF-16", Qt::CaseInsensitive)) {
    in.setCodec("UTF-16");
    in.setAutoDetectUnicode(true);
  } else {
    in.setCodec(qPrintable(fromCodec));
  }

  QStringList lines;
  while (!in.atEnd()) {
    lines << in.readLine();
  }
  f.close();

  // Filter lines
  QStringList filtered;
  foreach (QString line, lines) {
    bool found = false;
    foreach (QString word, wordList) {
      if (line.contains(word, Qt::CaseInsensitive)) {
        found = true;
        break;
      }
    }
    if (!found) {
      filtered << line;
    }
  }

  // Write back with same encoding
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    return false;

  QTextStream out(&f);
  if (fromCodec.startsWith("UTF-16", Qt::CaseInsensitive)) {
    out.setCodec("UTF-16");
    out.setGenerateByteOrderMark(true);
  } else {
    out.setCodec(qPrintable(fromCodec));
  }

  foreach (QString line, filtered) {
    out << line << "\n";
  }
  f.close();

  return true;
}

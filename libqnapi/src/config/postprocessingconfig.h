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

#ifndef POSTPROCESSINGCONFIG_H
#define POSTPROCESSINGCONFIG_H

#include <QString>
#include <QStringList>

enum EncodingChangeMethod {
  ECM_ORIGINAL = 0,
  ECM_CHANGE = 1,
  ECM_REPLACE_DIACRITICS = 2
};

class PostProcessingConfig {
 private:
  bool enabled_;
  EncodingChangeMethod encodingChangeMethod_;
  QString encodingFrom_;
  bool encodingAutoDetectFrom_;
  QString encodingTo_;
  bool showAllEncodings_;
  QString subFormat_;
  QString subExtension_;
  bool skipConvertAds_;
  bool removeLines_;
  QStringList removeLinesWords_;

 public:
  PostProcessingConfig() {}
  PostProcessingConfig(const bool& enabled,
                       const EncodingChangeMethod& encodingChangeMethod,
                       const QString& encodingFrom,
                       const bool& encodingAutoDetectFrom,
                       const QString& encodingTo, const bool& showAllEncodings,
                       const QString& subFormat, const QString& subExtension,
                       const bool& skipConvertAds,
                       const bool& removeLines,
                       const QStringList& removeLinesWords)
      : enabled_(enabled),
        encodingChangeMethod_(encodingChangeMethod),
        encodingFrom_(encodingFrom),
        encodingAutoDetectFrom_(encodingAutoDetectFrom),
        encodingTo_(encodingTo),
        showAllEncodings_(showAllEncodings),
        subFormat_(subFormat),
        subExtension_(subExtension),
        skipConvertAds_(skipConvertAds),
        removeLines_(removeLines),
        removeLinesWords_(removeLinesWords) {}

  bool enabled() const { return enabled_; }
  EncodingChangeMethod encodingChangeMethod() const {
    return encodingChangeMethod_;
  }
  QString encodingFrom() const { return encodingFrom_; }
  bool encodingAutoDetectFrom() const { return encodingAutoDetectFrom_; }
  QString encodingTo() const { return encodingTo_; }
  bool showAllEncodings() const { return showAllEncodings_; }
  QString subFormat() const { return subFormat_; }
  QString subExtension() const { return subExtension_; }
  bool skipConvertAds() const { return skipConvertAds_; }
  bool removeLines() const { return removeLines_; }
  QStringList removeLinesWords() const { return removeLinesWords_; }

  const PostProcessingConfig setEnabled(const bool& enabled) const {
    return PostProcessingConfig(
        enabled, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setEncodingChangeMethod(
      const EncodingChangeMethod& encodingChangeMethod) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setEncodingFrom(
      const QString& encodingFrom) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setEncodingAutoDetectFrom(
      const bool& encodingAutoDetectFrom) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setEncodingTo(const QString& encodingTo) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setShowAllEncodings(
      const bool& showAllEncodings) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setSubFormat(const QString& subFormat) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setSubExtension(
      const QString& subExtension) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension,
        skipConvertAds_, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setSkipConvertAds(
      const bool& skipConvertAds) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds, removeLines_, removeLinesWords_);
  }
  const PostProcessingConfig setRemoveLines(
      const bool& removeLines) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines, removeLinesWords_);
  }
  const PostProcessingConfig setRemoveLinesWords(
      const QStringList& removeLinesWords) const {
    return PostProcessingConfig(
        enabled_, encodingChangeMethod_, encodingFrom_, encodingAutoDetectFrom_,
        encodingTo_, showAllEncodings_, subFormat_, subExtension_,
        skipConvertAds_, removeLines_, removeLinesWords);
  }

  QString toString() const;
};

#endif

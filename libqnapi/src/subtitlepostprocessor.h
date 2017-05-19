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

#ifndef SUBTITLEPOSTPROCESSOR_H
#define SUBTITLEPOSTPROCESSOR_H

#include "config/postprocessingconfig.h"
#include "subconvert/subtitleconverter.h"
#include "utils/encodingutils.h"

#include <QSharedPointer>
#include <QString>

class SubtitlePostProcessor {
 public:
  SubtitlePostProcessor(
      const PostProcessingConfig& ppConfig,
      const QSharedPointer<const SubtitleConverter>& subtitleConverter);

  void perform(const QString& movieFilePath,
               const QString& subtitleFilePath) const;

 private:
  bool ppReplaceDiacriticsWithASCII(const QString& subtitleFilePath) const;
  bool ppChangeSubtitlesEncoding(const QString& subtitleFilePath,
                                 const QString& from, const QString& to) const;
  bool ppChangeSubtitlesEncoding(const QString& subtitleFilePath,
                                 const QString& to) const;
  bool ppRemoveLinesContainingWords(const QString& subtitleFilePath,
                                    QStringList wordList) const;

  const PostProcessingConfig& ppConfig;
  QSharedPointer<const SubtitleConverter> subtitleConverter;
  EncodingUtils encodingUtils;
};

#endif  // SUBTITLEPOSTPROCESSOR_H

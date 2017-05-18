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

#ifndef QSUBPOSTPROCESS_H
#define QSUBPOSTPROCESS_H

#include <QString>
#include "utils/encodingutils.h"

class QSubPostProcess
{
public:

    QSubPostProcess(QString _movieFilePath,
                    QString _subtitleFilePath)
        : movieFilePath(_movieFilePath),
          subtitleFilePath(_subtitleFilePath)
    {}

    void perform();

private:

    bool ppReplaceDiacriticsWithASCII();
    bool ppChangeSubtitlesEncoding(const QString & from, const QString & to);
    bool ppChangeSubtitlesEncoding(const QString & to);
    bool ppRemoveLinesContainingWords(QStringList wordList);

    QString movieFilePath;
    QString subtitleFilePath;

    EncodingUtils encodingUtils;
};

#endif // QSUBPOSTPROCESS_H

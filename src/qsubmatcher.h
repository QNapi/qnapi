#ifndef QSUBMATCHER_H
#define QSUBMATCHER_H

#include<QFileInfo>

class QSubMatcher
{
public:
    QSubMatcher(bool _noBackup,
                bool _isPostProcessingEnabled,
                QString _ppSubFormat,
                QString _ppSubExtension,
                bool _changePermissions,
                QString _changePermissionsTo)
        : noBackup(_noBackup),
          isPostProcessingEnabled(_isPostProcessingEnabled),
          ppSubFormat(_ppSubFormat),
          ppSubExtension(_ppSubExtension),
          changePermissions(_changePermissions),
          changePermissionsTo(_changePermissionsTo)
    {}

    bool matchSubtitles(QString subtitlesTmpFilePath, QString targetMovieFilePath) const;

private:

    QString selectTargetExtension(QFileInfo subtitlesTmpFileInfo) const;
    QString constructSubtitlePath(QString targetMovieFilePath, QString targetExtension, QString baseSuffix = "") const;
    bool isWritablePath(QString path) const;
    void removeOrCopy(QString targetMoviefilePath, QString targetSubtitlesFilePath) const;
    bool dryCopy(QString srcFilePath, QString dstFilePath) const;
    void fixFilePermissions(QString targetSubtitlesFilePath, QString changePermissionsTo) const;

    bool noBackup;
    bool isPostProcessingEnabled;
    QString ppSubFormat;
    QString ppSubExtension;
    bool changePermissions;
    QString changePermissionsTo;

};

#endif // QSUBMATCHER_H

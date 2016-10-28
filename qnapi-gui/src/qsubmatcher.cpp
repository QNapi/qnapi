#include "qsubmatcher.h"
#include "subconvert/subtitleformatsregistry.h"
#include <QDir>

bool QSubMatcher::matchSubtitles(QString subtitlesTmpFilePath, QString targetMovieFilePath) const
{
    QFileInfo subtitlesTmpFileInfo(subtitlesTmpFilePath);

    if(!subtitlesTmpFileInfo.exists())
        return false;

    QString targetExtension = selectTargetExtension(subtitlesTmpFileInfo);

    QString targetSubtitlesFilePath = constructSubtitlePath(targetMovieFilePath, targetExtension);

    if(!isWritablePath(targetSubtitlesFilePath))
        return false;

    removeOrCopy(targetMovieFilePath, targetSubtitlesFilePath);

    bool result = false;

#ifdef Q_OS_WIN
    // Pod windowsem, aby "wyczyscic" atrybuty pliku, tworzymy go na nowo
    result = dryCopy(subtitlesTmpFilePath, targetSubtitlesFilePath);
#else
    // pod innymi OS-ami wystarczy skopiowac plik
    result = QFile::copy(subtitlesTmpFilePath, targetSubtitlesFilePath);
#endif

    if(changePermissions)
    {
        fixFilePermissions(targetSubtitlesFilePath, changePermissionsTo);
    }

    return result;
}

QString QSubMatcher::selectTargetExtension(QFileInfo subtitlesTmpFileInfo) const
{
    QString targetExtension = subtitlesTmpFileInfo.suffix();

    if(isPostProcessingEnabled)
    {
        if(!ppSubFormat.isEmpty() && !ppSubExtension.isEmpty())
        {
            targetExtension = GlobalFormatsRegistry().select(ppSubFormat)->defaultExtension();
        }
        else if(!ppSubExtension.isEmpty())
        {
            targetExtension = ppSubExtension;
        }
    }

    return targetExtension;
}

QString QSubMatcher::constructSubtitlePath(QString targetMovieFilePath, QString targetExtension, QString baseSuffix) const
{
    QFileInfo targetMovieFileInfo(targetMovieFilePath);
    return targetMovieFileInfo.path() + QDir::separator() + targetMovieFileInfo.completeBaseName() + baseSuffix + "." + targetExtension;
}

bool QSubMatcher::isWritablePath(QString path) const
{
    QFileInfo pathFileInfo(path);
    return QFileInfo(pathFileInfo.absolutePath()).isWritable();
}

void QSubMatcher::removeOrCopy(QString targetMoviefilePath, QString targetSubtitlesFilePath) const
{
    if(QFile::exists(targetSubtitlesFilePath))
    {
        if(!noBackup)
        {
            QFileInfo targetSubtitlesFileInfo(targetSubtitlesFilePath);
            QString newName = constructSubtitlePath(targetMoviefilePath, targetSubtitlesFileInfo.suffix(), "_kopia");

            if(QFile::exists(newName))
                QFile::remove(newName);

            QFile::rename(targetSubtitlesFilePath, newName);
        }
        else
            QFile::remove(targetSubtitlesFilePath);
    }
}

bool QSubMatcher::dryCopy(QString srcFilePath, QString dstFilePath) const
{
    QFile dstFile(dstFilePath), srcFile(srcFilePath);
    bool result = false;

    if(!dstFile.open(QIODevice::WriteOnly) || !srcFile.open(QIODevice::ReadOnly))
    {
        dstFile.close();
    }
    else
    {
        result = dstFile.write(srcFile.readAll()) > 0;
        srcFile.close();
        dstFile.close();
    }

    return result;
}

void QSubMatcher::fixFilePermissions(QString targetSubtitlesFilePath, QString changePermissionsTo) const
{
    bool validPermissions;
    int permInt = changePermissionsTo.toInt(&validPermissions, 8);

    if(validPermissions)
    {
        int perm = 0;
        perm |= (permInt & 0700) << 2;
        perm |= (permInt & 0070) << 1;
        perm |= (permInt & 0007);
        QFile::setPermissions(targetSubtitlesFilePath, QFile::Permissions(perm));
    }
}

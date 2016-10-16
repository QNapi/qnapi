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
    QFile f(subtitles), f2(subtitlesTmp);
    if(!f.open(QIODevice::WriteOnly) || !f2.open(QIODevice::ReadOnly))
    {
        f.close();
    }
    else
    {
        result = f.write(f2.readAll()) > 0;
        f2.close();
        f.close();
    }
#else
    // pod normalnymi OS-ami nie trzeba sie gimnastykowac z atrybutami
    result = QFile::copy(subtitlesTmpFilePath, targetSubtitlesFilePath);

    // Zmiana uprawnien do pliku
    if(changePermissions)
    {
        bool validPermissions;
        int permInt = changePermissionsTo.toInt(&validPermissions, 8);

        if(validPermissions)
        {
            int perm = 0;
            perm |= (permInt & 0700) << 2;
            perm |= (permInt & 0070) << 1;
            perm |= (permInt & 0007);
            changeFilePermissions(targetSubtitlesFilePath, QFile::Permissions(perm));
        }
    }
#endif

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

bool QSubMatcher::changeFilePermissions(QString filePath, QFile::Permissions permissions) const
{
    if(!QFileInfo(filePath).exists())
        return false;

    return QFile::setPermissions(filePath, permissions);
}


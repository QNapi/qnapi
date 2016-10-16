#include "qsubmatcher.h"
#include "subconvert/subtitleformatsregistry.h"
#include <QDir>

bool QSubMatcher::matchSubtitles(QString subtitlesTmpFilePath, QString targetMovieFilePath) const
{
    QFileInfo subtitlesTmpFileInfo(subtitlesTmpFilePath);
    QFileInfo targetMovieFileInfo(targetMovieFilePath);

    if(!subtitlesTmpFileInfo.exists())
        return false;

    QString targetExtension = selectTargetExtension(subtitlesTmpFileInfo);

    QString targetSubtitlesFilePath = targetMovieFileInfo.path() + "/" + targetMovieFileInfo.completeBaseName() + "." + targetExtension;

    QFileInfo targetSubtitlesFileInfo(targetSubtitlesFilePath);

    if(!QFileInfo(targetSubtitlesFileInfo.absolutePath()).isWritable())
        return false;

    if(QFile::exists(targetSubtitlesFilePath))
    {
        if(!noBackup)
        {
            QString newName = targetMovieFileInfo.path() + QDir::separator() + targetMovieFileInfo.completeBaseName() + "_kopia." + targetSubtitlesFileInfo.suffix();

            if(QFile::exists(newName))
                QFile::remove(newName);

            QFile::rename(targetSubtitlesFilePath, newName);
        }
        else
            QFile::remove(targetSubtitlesFilePath);
    }

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
        r = f.write(f2.readAll()) > 0;
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

bool QSubMatcher::changeFilePermissions(QString filePath, QFile::Permissions permissions) const
{
    if(!QFileInfo(filePath).exists())
        return false;

    return QFile::setPermissions(filePath, permissions);
}


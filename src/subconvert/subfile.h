#ifndef SUBFILE_H
#define SUBFILE_H

#include <QStringList>
#include <QVector>

struct SubEntry
{
    long frameStart, frameStop;
    // if time-based, they represent frames as milliseconds
    // if frame-based, they represent exact frame numbers
    QStringList contentLines;
    QVector<int> boldLines, italicLines;
    QVector<std::pair<int,QString> > colorLines;
};

struct SubFile
{
    bool isTimeBased;
    QVector<SubEntry> entries;
};

#endif // SUBFILE_H


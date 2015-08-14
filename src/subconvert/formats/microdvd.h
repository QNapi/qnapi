#ifndef MICRODVD_H
#define MICRODVD_H

#include "subconvert/subtitleformat.h"
#include <QString>

class MicroDVDSubtitleFormat : public SubtitleFormat
{
public:
    bool isTimeBased() { return false; }
    QString formatName() { return "mDVD"; }
    QString defaultExtension() { return "txt"; }

    bool detect(const QStringList &lines)
    {
        // find first trimmed non-empty line
        // result = does it match to regexp {\d}{\d}(.*)
        //
        return false;
    }

    SubFile decode(const QStringList &lines)
    {
        SubFile sf;
        sf.isTimeBased = false;
        // match each trimmed non-empty line to regexp {\d}{\d}(.*)
        // extract SubEntry, add to sf.entries
        //sf.entries
    }

    QStringList encode(const SubFile & subFile)
    {
        QStringList lines;
        foreach(SubEntry e, subFile.entries)
        {
            QString line = QString("{%1}{%2}%3")
                    .arg(e.frameStart)
                    .arg(e.frameStop)
                    .arg(e.contentLines.join('|'));
            lines.append(line);
        }
        return lines;
    }
};

#endif // MICRODVD_H


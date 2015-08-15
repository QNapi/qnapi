#include "microdvd.h"

bool MicroDVDSubtitleFormat::detect(const QStringList &lines)
{
    // find first trimmed non-empty line
    // result = does it match to regexp {\d}{\d}(.*)
    //
    return false;
}

SubFile MicroDVDSubtitleFormat::decode(const QStringList &lines)
{
    SubFile sf;
    sf.isTimeBased = false;
    // match each trimmed non-empty line to regexp {\d}{\d}(.*)
    // extract SubEntry, add to sf.entries
    //sf.entries
    return sf;
}

QStringList MicroDVDSubtitleFormat::encode(const SubFile & subFile)
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

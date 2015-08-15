#include "subtitleformatsregistry.h"

#include "subconvert/formats/microdvd.h"
#include "subconvert/formats/mpl2.h"
#include "subconvert/formats/subrip.h"
#include "subconvert/formats/tmplayer.h"

SubtitleFormatsRegistry::SubtitleFormatsRegistry()
{}

SubtitleFormatsRegistry::~SubtitleFormatsRegistry()
{
    foreach(QString formatName, formats.keys())
    {
        delete formats[formatName];
    }
}

void SubtitleFormatsRegistry::registerFormat(SubtitleFormat *format)
{
    formats.insert(format->formatName(), format);
}

QStringList SubtitleFormatsRegistry::enumerateFormats() const
{
    return formats.keys();
}

SubtitleFormat* SubtitleFormatsRegistry::select(const QString & format) const
{
    return formats[format];
}


SubtitleFormatsRegistry & GlobalFormatsRegistry()
{
    static SubtitleFormatsRegistry registry;
    if(registry.enumerateFormats().isEmpty())
    {
        registry.registerFormat(new MicroDVDSubtitleFormat);
        registry.registerFormat(new MPL2SubtitleFormat);
        registry.registerFormat(new SubRipSubtitleFormat);
        registry.registerFormat(new TMPlayerSubtitleFormat);
    }
    return registry;
}

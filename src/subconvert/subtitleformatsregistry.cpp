#include "subtitleformatsregistry.h"

#include "subconvert/formats/microdvd.h"

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

    }
    return registry;
}

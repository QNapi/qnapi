QT += network xml
QT -= gui

TARGET = qnapi
TEMPLATE = lib

CONFIG += staticlib \
          warn_on \
          silent \
          c++11

DESTDIR = $$PWD

INCLUDEPATH += $$PWD/src

include(../deps/libmaia/maia.pri)
include(../deps/qt-maybe/qt-maybe.pri)

UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp

SOURCES += src/config/configreader.cpp \
    src/config/configwriter.cpp \
    src/config/engineconfig.cpp \
    src/config/generalconfig.cpp \
    src/config/postprocessingconfig.cpp \
    src/config/qnapiconfig.cpp \
    src/config/scanconfig.cpp \
    src/config/staticconfig.cpp \
    src/engines/napiprojektdownloadengine.cpp \
    src/engines/napisy24downloadengine.cpp \
    src/engines/opensubtitlesdownloadengine.cpp \
    src/engines/subtitledownloadengine.cpp \
    src/engines/subtitledownloadenginesregistry.cpp \
    src/libqnapi.cpp \
    src/movieinfo/libmediainfomovieinfoprovider.cpp \
    src/subconvert/formats/microdvd.cpp \
    src/subconvert/formats/mpl2.cpp \
    src/subconvert/formats/subrip.cpp \
    src/subconvert/formats/tmplayer.cpp \
    src/subconvert/subtitleconverter.cpp \
    src/subconvert/subtitleformat.cpp \
    src/subconvert/subtitleformatsregistry.cpp \
    src/subtitleinfo.cpp \
    src/subtitlelanguage.cpp \
    src/subtitlematcher.cpp \
    src/subtitlepostprocessor.cpp \
    src/utils/encodingutils.cpp \
    src/utils/p7zipdecoder.cpp \
    src/utils/synchttp.cpp \
    src/utils/syncxmlrpc.cpp


HEADERS += src/config/configreader.h \
    src/config/configwriter.h \
    src/config/engineconfig.h \
    src/config/generalconfig.h \
    src/config/postprocessingconfig.h \
    src/config/qnapiconfig.h \
    src/config/scanconfig.h \
    src/config/staticconfig.h \
    src/engines/napiprojektdownloadengine.h \
    src/engines/napisy24downloadengine.h \
    src/engines/opensubtitlesdownloadengine.h \
    src/engines/subtitledownloadengine.h \
    src/engines/subtitledownloadenginemetadata.h \
    src/engines/subtitledownloadenginesregistry.h \
    src/libqnapi.h \
    src/movieinfo/libmediainfomovieinfoprovider.h \
    src/movieinfo/movieinfo.h \
    src/movieinfo/movieinfoprovider.h \
    src/subconvert/formats/microdvd.h \
    src/subconvert/formats/mpl2.h \
    src/subconvert/formats/subrip.h \
    src/subconvert/formats/tmplayer.h \
    src/subconvert/subfile.h \
    src/subconvert/subtitleconverter.h \
    src/subconvert/subtitleformat.h \
    src/subconvert/subtitleformatsregistry.h \
    src/subtitleinfo.h \
    src/subtitlelanguage.h \
    src/subtitlematcher.h \
    src/subtitlepostprocessor.h \
    src/utils/encodingutils.h \
    src/utils/p7zipdecoder.h \
    src/utils/synchttp.h \
    src/utils/syncxmlrpc.h \
    src/version.h

unix:!macx {
    PKGCONFIG += libmediainfo
}

macx {
    INCLUDEPATH += ../deps/libmediainfo/include
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.7
}

win32 {
    INCLUDEPATH += ../deps/libmediainfo/include
}

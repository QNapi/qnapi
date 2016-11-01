QT += network xml
QT -= gui

TARGET = qnapi
TEMPLATE = lib

CONFIG += staticlib \
          warn_on \
          release \
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

SOURCES += src/libqnapi.cpp \
    src/utils/synchttp.cpp \
    src/engines/qnapiabstractengine.cpp \
    src/engines/qnapiprojektengine.cpp \
    src/engines/qnapisy24engine.cpp \
    src/engines/qopensubtitlesengine.cpp \
    src/subconvert/subtitleformatsregistry.cpp \
    src/subconvert/subtitleconverter.cpp \
    src/subconvert/formats/microdvd.cpp \
    src/subconvert/formats/tmplayer.cpp \
    src/subconvert/formats/subrip.cpp \
    src/subconvert/formats/mpl2.cpp \
    src/subconvert/subtitleformat.cpp \
    src/qnapiconfig.cpp \
    src/qnapilanguage.cpp \
    src/syncxmlrpc.cpp \
    src/encodingutils.cpp \
    src/libmediainfomovieinfoparser.cpp \
    src/qsubpostprocess.cpp \
    src/qnapisubtitleinfo.cpp \
    src/qsubmatcher.cpp


HEADERS += src/libqnapi.h \
    src/version.h \
    src/utils/synchttp.h \
    src/engines/qnapiabstractengine.h \
    src/engines/qnapiprojektengine.h \
    src/engines/qnapisy24engine.h \
    src/engines/qopensubtitlesengine.h \
    src/subconvert/subfile.h \
    src/subconvert/subtitleformat.h \
    src/subconvert/formats/microdvd.h \
    src/subconvert/subtitleformatsregistry.h \
    src/subconvert/subtitleconverter.h \
    src/subconvert/formats/tmplayer.h \
    src/subconvert/formats/subrip.h \
    src/subconvert/formats/mpl2.h \
    src/qnapiconfig.h \
    src/movieinfo.h \
    src/qnapilanguage.h \
    src/syncxmlrpc.h \
    src/qnapisubtitleinfo.h \
    src/encodingutils.h \
    src/libmediainfomovieinfoparser.h \
    src/qsubpostprocess.h \
    src/qsubmatcher.h


unix {
    PKGCONFIG += libmediainfo
}

macx {
    INCLUDEPATH += ../deps/libmediainfo/include
}

win32 {
    INCLUDEPATH += ../deps/libmediainfo/include
}

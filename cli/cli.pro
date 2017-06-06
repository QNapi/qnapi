TEMPLATE = app

CONFIG += warn_on qt resources silent c++11

QT += core network xml
QT -= gui

SOURCES += src/main.cpp \
    src/commandargsparser.cpp \
    src/clisubtitlesdownloader.cpp \
    src/console.cpp
    
HEADERS += src/qnapiclicommand.h \
    src/commandargsparser.h \
    src/clisubtitlesdownloader.h \
    src/namespace_tr.h \
    src/console.h

RESOURCES += res/resources.qrc

MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(../libqnapi/libqnapi.pri)

unix {
    TARGET = qnapic
    DESTDIR = ../
    CONFIG += link_pkgconfig
    PKGCONFIG += libmediainfo
}

macx {
    CONFIG -= app_bundle
    LIBS += -framework CoreFoundation
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.7
}

win32 {
    CONFIG += nostrip

    RC_FILE = ../win32/qnapi.rc

    TARGET = qnapic

    target.path = ../win32/out
    INSTALLS += target
}


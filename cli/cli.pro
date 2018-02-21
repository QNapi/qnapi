TEMPLATE = app

CONFIG += warn_on qt resources silent c++11 console

QT += core network xml
QT -= gui

SOURCES += src/main.cpp \
    src/commandargsparser.cpp \
    src/clisubtitlesdownloader.cpp \
    src/console.cpp \
    src/climain.cpp
    
HEADERS += src/qnapiclicommand.h \
    src/commandargsparser.h \
    src/clisubtitlesdownloader.h \
    src/console.h \
    src/climain.h

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
    QT_CONFIG -= no-pkg-config
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


TEMPLATE = app

CONFIG += warn_on qt resources silent c++11 console

QT += core network xml
QT -= gui

SOURCES += src/main.cpp \
    src/clisubtitlesdownloader.cpp \
    src/climain.cpp
    
HEADERS += \
    src/clisubtitlesdownloader.h \
    src/climain.h

RESOURCES += res/resources.qrc

MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(../qnapi.pri)
include(../libqnapi/libqnapi.pri)

unix {
    TARGET = qnapic
    DESTDIR = $${OUTDIR}
    CONFIG += link_pkgconfig
    PKGCONFIG += libmediainfo
}

macx {
    QT_CONFIG -= no-pkg-config
    CONFIG -= app_bundle
    LIBS += -framework CoreFoundation
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.8
}

!win32 {
    target.path = $${INSTALL_PREFIX}/bin
    INSTALLS += target
}

win32 {
    CONFIG += nostrip
    RC_FILE = $${OUTDIR}/qnapi.rc
    TARGET = qnapic

    target.path = $${INSTALL_PREFIX}
    INSTALLS += target
}

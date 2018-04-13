TEMPLATE = app

CONFIG += warn_on qt resources silent c++11

QT += core network xml gui widgets

SOURCES += src/main.cpp \
    src/forms/frmprogress.cpp \
    src/forms/frmlistsubtitles.cpp \
    src/forms/frmsummary.cpp \
    src/forms/frmscan.cpp \
    src/forms/frmoptions.cpp \
    src/forms/frmabout.cpp \
    src/forms/frmnapiprojektconfig.cpp \
    src/forms/frmopensubtitlesconfig.cpp \
    src/forms/frmnapisy24config.cpp \
    src/forms/frmconvert.cpp \
    src/forms/subdatawidget.cpp \
    src/qcumber/qmanagedrequest.cpp \
    src/qcumber/qmanagedsocket.cpp \
    src/qcumber/qsingleapplication.cpp \
    src/qnapiopendialog.cpp \
    src/qnapiapp.cpp \
    src/guimain.cpp

HEADERS += src/forms/frmprogress.h \
    src/forms/frmlistsubtitles.h \
    src/forms/frmsummary.h \
    src/forms/frmscan.h \
    src/forms/frmoptions.h \
    src/forms/frmabout.h \
    src/forms/frmnapiprojektconfig.h \
    src/forms/frmopensubtitlesconfig.h \
    src/forms/frmnapisy24config.h \
    src/forms/frmconvert.h \
    src/forms/subdatawidget.h \
    src/qcumber/qmanagedrequest.h \
    src/qcumber/qmanagedsocket.h \
    src/qcumber/qsingleapplication.h \
    src/qcumber/qcumber.h \
    src/qcheckedlistwidget.h \
    src/qnapithread.h \
    src/qnapiopendialog.h \
    src/qnapiapp.h \
    src/guimain.h

FORMS += ui/frmprogress.ui \
    ui/frmlistsubtitles.ui \
    ui/frmsummary.ui \
    ui/frmscan.ui \
    ui/frmoptions.ui \
    ui/frmabout.ui \
    ui/frmconvert.ui \
    ui/subDataWidget.ui \
    ui/napiprojekt/frmnapiprojektconfig.ui \
    ui/opensubtitles/frmopensubtitlesconfig.ui \
    ui/napisy24/frmnapisy24config.ui

RESOURCES += res/resources.qrc
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(../qnapi.pri)
include(../libqnapi/libqnapi.pri)

unix:!macx {
    TARGET = qnapi
    DESTDIR = ../
    CONFIG += link_pkgconfig
    PKGCONFIG += libmediainfo
}

macx {
    SOURCES += src/utils/infoplistdockicon.cpp
    HEADERS += src/utils/infoplistdockicon.h

    TARGET = QNapi
    DESTDIR = $${OUTDIR}

    LIBS += -framework CoreFoundation
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.8
    ICON = $${OUTDIR}/qnapi.icns
    QMAKE_INFO_PLIST = $${OUTDIR}/Info.plist
    7ZIP_BINARY.files = $${OUTDIR}/content/7za
    7ZIP_BINARY.path = Contents/Resources
    LIBMEDIAINFO.files = ../deps/libmediainfo/lib/libmediainfo.0.dylib
    LIBMEDIAINFO.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += 7ZIP_BINARY LIBMEDIAINFO
}

!win32 {
    SOURCES += src/qcumber/qinterprocesschannel.cpp
    HEADERS += src/qcumber/qinterprocesschannel.h

    !macx {
        target.path = $${INSTALL_PREFIX}/bin
        INSTALLS += target
    }
}

win32 {
    CONFIG += nostrip
    SOURCES += src/qcumber/qinterprocesschannel_win32.cpp
    HEADERS += src/qcumber/qinterprocesschannel_win32.h
    RC_FILE = $${OUTDIR}/qnapi.rc
    TARGET = qnapi

    target.path = $${INSTALL_PREFIX}
    INSTALLS += target
}

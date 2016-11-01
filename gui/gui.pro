TEMPLATE = app

CONFIG += warn_on \
    thread \
    qt \
    resources \
    release \
    silent \
    c++11

QT += network \
    gui \
    widgets \
    core \
    xml

SOURCES += src/main.cpp \
    src/engines/qnapiabstractengine.cpp \
    src/engines/qnapiprojektengine.cpp \
    src/engines/qnapisy24engine.cpp \
    src/engines/qopensubtitlesengine.cpp \
    src/forms/frmprogress.cpp \
    src/forms/frmlistsubtitles.cpp \
    src/forms/frmsummary.cpp \
    src/forms/frmscan.cpp \
    src/forms/frmoptions.cpp \
    src/forms/frmabout.cpp \
    src/forms/frmnapiprojektconfig.cpp \
    src/forms/frmopensubtitlesconfig.cpp \
    src/forms/frmnapisy24config.cpp \
    src/qcumber/qmanagedrequest.cpp \
    src/qcumber/qmanagedsocket.cpp \
    src/qcumber/qsingleapplication.cpp \
    src/qnapiconfig.cpp \
    src/qnapiapp.cpp \
    src/qnapicli.cpp \
    src/qnapi.cpp \
    src/qnapiopendialog.cpp \
    src/qnapilanguage.cpp \
    src/syncxmlrpc.cpp \
    src/encodingutils.cpp \
    src/forms/frmconvert.cpp \
    src/subconvert/subtitleformatsregistry.cpp \
    src/subconvert/subtitleconverter.cpp \
    src/subconvert/formats/microdvd.cpp \
    src/subconvert/formats/tmplayer.cpp \
    src/subconvert/formats/subrip.cpp \
    src/subconvert/formats/mpl2.cpp \
    src/subconvert/subtitleformat.cpp \
    src/libmediainfomovieinfoparser.cpp \
    src/qsubpostprocess.cpp \
    src/forms/subdatawidget.cpp \
    src/qnapisubtitleinfo.cpp \
    src/qsubmatcher.cpp

HEADERS += src/engines/qnapiabstractengine.h \
    src/engines/qnapiprojektengine.h \
    src/engines/qnapisy24engine.h \
    src/engines/qopensubtitlesengine.h \
    src/forms/frmprogress.h \
    src/forms/frmlistsubtitles.h \
    src/forms/frmsummary.h \
    src/forms/frmscan.h \
    src/forms/frmoptions.h \
    src/forms/frmabout.h \
    src/forms/frmnapiprojektconfig.h \
    src/forms/frmopensubtitlesconfig.h \
    src/forms/frmnapisy24config.h \
    src/qcumber/qmanagedrequest.h \
    src/qcumber/qmanagedsocket.h \
    src/qcumber/qsingleapplication.h \
    src/qcumber/qcumber.h \
    src/qnapiconfig.h \
    src/qnapiapp.h \
    src/qnapicli.h \
    src/qnapi.h \
    src/movieinfo.h \
    src/qnapiopendialog.h \
    src/qnapilanguage.h \
    src/qcheckedlistwidget.h \
    src/qnapithread.h \
    src/syncxmlrpc.h \
    src/qnapisubtitleinfo.h \
    src/encodingutils.h \
    src/forms/frmconvert.h \
    src/subconvert/subfile.h \
    src/subconvert/subtitleformat.h \
    src/subconvert/formats/microdvd.h \
    src/subconvert/subtitleformatsregistry.h \
    src/subconvert/subtitleconverter.h \
    src/subconvert/formats/tmplayer.h \
    src/subconvert/formats/subrip.h \
    src/subconvert/formats/mpl2.h \
    src/libmediainfomovieinfoparser.h \
    src/qsubpostprocess.h \
    src/forms/subdatawidget.h \
    src/qsubmatcher.h

FORMS += ui/frmprogress.ui \
    ui/frmlistsubtitles.ui \
    ui/frmsummary.ui \
    ui/frmscan.ui \
    ui/frmoptions.ui \
    ui/frmabout.ui \
    ui/napiprojekt/frmnapiprojektconfig.ui \
    ui/opensubtitles/frmopensubtitlesconfig.ui \
    ui/napisy24/frmnapisy24config.ui \
    ui/frmconvert.ui \
    ui/subDataWidget.ui

RESOURCES += res/resources.qrc
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(../deps/libmaia/maia.pri)
include(../deps/qt-maybe/qt-maybe.pri)
include(../libqnapi/libqnapi.pri)

unix { 
    CONFIG += link_pkgconfig
    PKGCONFIG += libmediainfo
    TARGET = qnapi
    DESTDIR = ../
}

macx {
    CONFIG -= link_pkgconfig
    INCLUDEPATH += ../deps/libmediainfo/include
    LIBS += -framework CoreFoundation

    TARGET = QNapi
    DESTDIR = ../macx/

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.7
    ICON = ../macx/qnapi.icns
    QMAKE_INFO_PLIST = ../macx/Info.plist
    7ZIP_BINARY.files = ../macx/content/7za
    7ZIP_BINARY.path = Contents/Resources
    LIBMEDIAINFO.files = ../deps/libmediainfo/lib/libmediainfo.0.dylib
    LIBMEDIAINFO.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += 7ZIP_BINARY LIBMEDIAINFO
}

win32 {
    CONFIG += nostrip

    INCLUDEPATH += ../deps/libmediainfo/include
    SOURCES += src/qcumber/qinterprocesschannel_win32.cpp
    HEADERS += src/qcumber/qinterprocesschannel_win32.h

    RC_FILE = ../win32/qnapi.rc

    TARGET = qnapi

    target.path = ../win32/out
    INSTALLS += target
}

!win32 { 
    SOURCES += src/qcumber/qinterprocesschannel.cpp
    HEADERS += src/qcumber/qinterprocesschannel.h
}

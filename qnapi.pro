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
    src/qcumber/qmanagedrequest.cpp \
    src/qcumber/qmanagedsocket.cpp \
    src/qcumber/qsingleapplication.cpp \
    src/qnapiconfig.cpp \
    src/qnapiapp.cpp \
    src/qnapicli.cpp \
    src/qnapi.cpp \
    src/qmultiparthttprequest.cpp \
    src/qnapiopendialog.cpp \
    src/qnapilanguage.cpp \
    src/synchttp.cpp \
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
    src/forms/frmmain.cpp


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
    src/qmultiparthttprequest.h \
    src/qnapithread.h \
    src/synchttp.h \
    src/syncxmlrpc.h \
    src/qnapisubtitleinfo.h \
    src/version.h \
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
    src/forms/frmmain.h

FORMS += ui/frmprogress.ui \
    ui/frmlistsubtitles.ui \
    ui/frmsummary.ui \
    ui/frmscan.ui \
    ui/frmoptions.ui \
    ui/frmabout.ui \
    ui/napiprojekt/frmnapiprojektconfig.ui \
    ui/opensubtitles/frmopensubtitlesconfig.ui \
    ui/frmconvert.ui \
    ui/subDataWidget.ui \
    ui/frmmain.ui

RESOURCES += res/resources.qrc
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(deps/libmaia/maia.pri)
include(deps/qt-maybe/qt-maybe.pri)

unix { 
    CONFIG += link_pkgconfig
    PKGCONFIG += libmediainfo
    INSTALL_PREFIX = /usr
    DATADIR=$${INSTALL_PREFIX}/share
    target.path = $${INSTALL_PREFIX}/bin
    doc.path = $${INSTALL_PREFIX}/share/doc/$${TARGET}
    doc.files = doc/ChangeLog \
        doc/LICENSE \
        doc/LICENSE-pl \
        doc/COPYRIGHT \
        doc/qnapi-download.desktop \
        doc/qnapi-scan.desktop \
        doc/qnapi-download.schemas \
        doc/qnapi-scan.schemas
    man.path = $${INSTALL_PREFIX}/share/man/man1
    man.files = doc/$${TARGET}.1.gz
    icons16.files = res/icons/16x16/apps/qnapi.png
    icons16.path = $${DATADIR}/icons/hicolor/16x16/apps/
    icons22.files = res/icons/22x22/apps/qnapi.png
    icons22.path = $${DATADIR}/icons/hicolor/22x22/apps/
    icons32.files = res/icons/32x32/apps/qnapi.png
    icons32.path = $${DATADIR}/icons/hicolor/32x32/apps/
    icons48.files = res/icons/48x48/apps/qnapi.png
    icons48.path = $${DATADIR}/icons/hicolor/48x48/apps/
    icons128.files = res/icons/128x128/apps/qnapi.png
    icons128.path = $${DATADIR}/icons/hicolor/128x128/apps/
    icons512.files = res/icons/512x512/apps/qnapi.png
    icons512.path = $${DATADIR}/icons/hicolor/512x512/apps/
    INSTALLS += icons16 icons22 icons32 icons48 icons128 icons512
    desktop.path = $${INSTALL_PREFIX}/share/applications
    desktop.files = doc/$${TARGET}.desktop
    INSTALLS += target \
        doc \
        man \
        desktop
}

macx {
    CONFIG -= link_pkgconfig
    INCLUDEPATH += deps/libmediainfo/include
    LIBS += -framework CoreFoundation

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS_X86_64 = -mmacosx-version-min=10.7
    ICON = macx/qnapi.icns
    QMAKE_INFO_PLIST = macx/Info.plist
    TARGET = macx/QNapi
    7ZIP_BINARY.files = macx/content/7za
    7ZIP_BINARY.path = Contents/Resources
    LIBMEDIAINFO.files = deps/libmediainfo/lib/libmediainfo.0.dylib
    LIBMEDIAINFO.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += 7ZIP_BINARY LIBMEDIAINFO

    macdeploy.commands = macdeployqt macx/QNapi.app
    appdmg.depends = macdeploy
    appdmg.commands = appdmg macx/appdmg.json macx/QNapi.dmg

    QMAKE_EXTRA_TARGETS += macdeploy appdmg
}

win32 {
    INCLUDEPATH += deps/libmediainfo/include

    RC_FILE = win32/qnapi.rc
    SOURCES += src/qcumber/qinterprocesschannel_win32.cpp
    HEADERS += src/qcumber/qinterprocesschannel_win32.h

    CONFIG += nostrip

    INSTALL_PREFIX = win32/out

    target.path = $${INSTALL_PREFIX}

    p7zip.files += win32/content/7za.exe
    p7zip.path = $${INSTALL_PREFIX}

    doc.files = doc/ChangeLog \
        doc/LICENSE \
        doc/LICENSE-pl
    doc.path = $${INSTALL_PREFIX}

    icudlls.files += $$[QT_INSTALL_BINS]/icuin54.dll
    icudlls.files += $$[QT_INSTALL_BINS]/icuuc54.dll
    icudlls.files += $$[QT_INSTALL_BINS]/icudt54.dll
    icudlls.path = $${INSTALL_PREFIX}

    libmediainfodlls.files += deps/libmediainfo/bin/MediaInfo.dll
    libmediainfodlls.path = $${INSTALL_PREFIX}

    deploywin.commands = windeployqt --no-translations --no-quick-import --no-system-d3d-compiler --no-angle --no-webkit --no-webkit2 win32\out\qnapi.exe

    platform.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    platform.path = $${INSTALL_PREFIX}/platforms
    platform.depends = deploywin

    QMAKE_EXTRA_TARGETS += icudlls libmediainfodlls deploywin platform

    INSTALLS = target p7zip doc icudlls libmediainfodlls platform
}

!win32 { 
    SOURCES += src/qcumber/qinterprocesschannel.cpp
    HEADERS += src/qcumber/qinterprocesschannel.h
}

TEMPLATE = app

CONFIG += warn_on \
    thread \
    qt \
    resources \
    release \
    silent

QT += network \
    gui \
    widgets \
    core \
    xml

SOURCES += src/main.cpp \
    src/forms/frmprogress.cpp \
    src/forms/frmlistsubtitles.cpp \
    src/forms/frmsummary.cpp \
    src/forms/frmscan.cpp \
    src/forms/frmoptions.cpp \
    src/forms/frmabout.cpp \
    src/forms/frmnapiprojektconfig.cpp \
    src/forms/frmupload.cpp \
    src/forms/frmcorrect.cpp \
    src/forms/frmreport.cpp \
    src/forms/frmopensubtitlesconfig.cpp \
    src/qcumber/qmanagedrequest.cpp \
    src/qcumber/qmanagedsocket.cpp \
    src/qcumber/qsingleapplication.cpp \
    src/qnapiconfig.cpp \
    src/qnapiapp.cpp \
    src/qnapicli.cpp \
    src/qnapi.cpp \
    src/qnapiabstractengine.cpp \
    src/qnapiprojektengine.cpp \
    src/qopensubtitlesengine.cpp \
    src/qmultiparthttprequest.cpp \
    src/movieinfo.cpp \
    src/qnapiopendialog.cpp \
    src/qnapilanguage.cpp \
    src/qnapisy24engine.cpp \
    src/synchttp.cpp \
    src/syncxmlrpc.cpp \
    src/encodingutils.cpp

HEADERS += src/forms/frmprogress.h \
    src/forms/frmlistsubtitles.h \
    src/forms/frmsummary.h \
    src/forms/frmscan.h \
    src/forms/frmoptions.h \
    src/forms/frmabout.h \
    src/forms/frmnapiprojektconfig.h \
    src/forms/frmupload.h \
    src/forms/frmcorrect.h \
    src/forms/frmreport.h \
    src/forms/frmopensubtitlesconfig.h \
    src/qcumber/qmanagedrequest.h \
    src/qcumber/qmanagedsocket.h \
    src/qcumber/qsingleapplication.h \
    src/qcumber/qcumber.h \
    src/qnapiconfig.h \
    src/qnapiapp.h \
    src/qnapicli.h \
    src/qnapi.h \
    src/qnapiabstractengine.h \
    src/qnapiprojektengine.h \
    src/qopensubtitlesengine.h \
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
    src/qnapisy24engine.h \
    src/encodingutils.h

FORMS += ui/frmprogress.ui \
    ui/frmlistsubtitles.ui \
    ui/frmsummary.ui \
    ui/frmscan.ui \
    ui/frmoptions.ui \
    ui/frmabout.ui \
    ui/napiprojekt/frmnapiprojektconfig.ui \
    ui/frmupload.ui \
    ui/frmcorrect.ui \
    ui/frmreport.ui \
    ui/opensubtitles/frmopensubtitlesconfig.ui

RESOURCES += res/resources.qrc
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src

include(deps/libmaia/maia.pri)

macx { 
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
    ICON = macx/qnapi.icns
    QMAKE_INFO_PLIST = macx/Info.plist
    TARGET = macx/QNapi
    7ZIP_BINARY.files = macx/content/7za
    7ZIP_BINARY.path = Contents/Resources
    QMAKE_BUNDLE_DATA += 7ZIP_BINARY

    macdeploy.commands = macdeployqt macx/QNapi.app
    appdmg.depends = macdeploy
    appdmg.commands = appdmg macx/appdmg.json macx/QNapi.dmg

    QMAKE_EXTRA_TARGETS += macdeploy appdmg
}

unix { 
    INSTALL_PREFIX = /usr
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
    icons.path = /usr/share/icons
    icons.files = res/qnapi.png \
        res/qnapi-48.png \
        res/qnapi-128.png \
        res/qnapi-512.png
    desktop.path = /usr/share/applications
    desktop.files = doc/$${TARGET}.desktop
    INSTALLS = target \
        doc \
        man \
        icons \
        desktop
}

win32 { 
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

    deploywin.commands = windeployqt --no-plugins --no-translations --no-quick-import --no-system-d3d-compiler --no-angle --no-webkit --no-webkit2 win32\out\qnapi.exe

    platform.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    platform.path = $${INSTALL_PREFIX}/platforms
    platform.depends = deploywin

    QMAKE_EXTRA_TARGETS += deploywin platform

    INSTALLS = target p7zip doc platform
}

!win32 { 
    SOURCES += src/qcumber/qinterprocesschannel.cpp
    HEADERS += src/qcumber/qinterprocesschannel.h
}

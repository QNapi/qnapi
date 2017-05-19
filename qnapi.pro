TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libqnapi gui

TRANSLATIONS += translations/qnapi_it.ts translations/qnapi_pl.ts

unix {
    INSTALL_PREFIX = /usr
    DATADIR=$${INSTALL_PREFIX}/share
    target.files += qnapi
    target.path = $${INSTALL_PREFIX}/bin
    doc.path = $${INSTALL_PREFIX}/share/doc/qnapi
    doc.files = doc/ChangeLog \
        doc/LICENSE \
        doc/LICENSE-pl \
        doc/COPYRIGHT \
        doc/qnapi-download.desktop \
        doc/qnapi-scan.desktop \
        doc/qnapi-download.schemas \
        doc/qnapi-scan.schemas
    man.path = $${INSTALL_PREFIX}/share/man/man1/
    man.files = doc/man/qnapi.1.gz
    man_it.path = $${INSTALL_PREFIX}/share/man/it/man1/
    man_it.files = doc/man/it/qnapi.1.gz
    man_pl.path = $${INSTALL_PREFIX}/share/man/pl/man1/
    man_pl.files = doc/man/pl/qnapi.1.gz
    icons16.files = gui/res/icons/16x16/apps/qnapi.png
    icons16.path = $${DATADIR}/icons/hicolor/16x16/apps/
    icons32.files = gui/res/icons/32x32/apps/qnapi.png
    icons32.path = $${DATADIR}/icons/hicolor/32x32/apps/
    icons48.files = gui/res/icons/48x48/apps/qnapi.png
    icons48.path = $${DATADIR}/icons/hicolor/48x48/apps/
    icons128.files = gui/res/icons/128x128/apps/qnapi.png
    icons128.path = $${DATADIR}/icons/hicolor/128x128/apps/
    icons512.files = gui/res/icons/512x512/apps/qnapi.png
    icons512.path = $${DATADIR}/icons/hicolor/512x512/apps/
    INSTALLS += icons16 icons32 icons48 icons128 icons512

    desktop.path = $${INSTALL_PREFIX}/share/applications
    desktop.files = doc/qnapi.desktop
    INSTALLS += target doc man man_it man_pl desktop
}

macx {
    macdeploy.commands = macdeployqt macx/QNapi.app
    appdmg.depends = macdeploy
    appdmg.commands = appdmg macx/appdmg.json macx/QNapi.dmg

    QMAKE_EXTRA_TARGETS += macdeploy appdmg
}

win32 {
    INSTALL_PREFIX = win32/out

    QMAKE_STRIP = echo

    p7zip.files = win32/content/7za.exe
    p7zip.path = $${INSTALL_PREFIX}

    doc.files = doc/ChangeLog \
                doc/LICENSE \
                doc/LICENSE-pl
    doc.path = $${INSTALL_PREFIX}

    icudlls.files = $$[QT_INSTALL_BINS]/icuin54.dll \
                    $$[QT_INSTALL_BINS]/icuuc54.dll \
                    $$[QT_INSTALL_BINS]/icudt54.dll
    icudlls.path = $${INSTALL_PREFIX}

    libmediainfodlls.files += deps/libmediainfo/bin/MediaInfo.dll
    libmediainfodlls.path = $${INSTALL_PREFIX}

    deploywin.commands = windeployqt --no-translations --no-quick-import --no-system-d3d-compiler --no-angle --no-webkit --no-webkit2 win32/out/qnapi.exe

    platform.files = $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    platform.path = $${INSTALL_PREFIX}/platforms
    platform.depends = deploywin

    QMAKE_EXTRA_TARGETS += deploywin platform

    INSTALLS = p7zip doc icudlls libmediainfodlls platform
}

doxygen.commands = rm -fr doc/doxygen/* && doxygen Doxyfile && echo "Doxygen documentation generated in doc/doxygen/index.html"
QMAKE_EXTRA_TARGETS += doxygen


TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libqnapi qnapi-gui

macx {
    macdeploy.commands = macdeployqt macx/QNapi.app
    appdmg.depends = macdeploy
    appdmg.commands = appdmg macx/appdmg.json macx/QNapi.dmg

    QMAKE_EXTRA_TARGETS += macdeploy appdmg
}



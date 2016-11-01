LIBS += -L$$PWD -lqnapi
INCLUDEPATH += $$PWD/src \
    $$PWD/src/utils \
    $$PWD/../deps/libmaia \
    $$PWD/../deps/qt-maybe
DEPENDPATH += $$PWD/src $$PWD/src/utils
PRE_TARGETDEPS += $$PWD/libqnapi.a


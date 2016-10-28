LIBS += -L$$PWD -lqnapi
INCLUDEPATH += $$PWD/src $$PWD/src/utils
DEPENDPATH += $$PWD/src $$PWD/src/utils
PRE_TARGETDEPS += $$PWD/libqnapi.a


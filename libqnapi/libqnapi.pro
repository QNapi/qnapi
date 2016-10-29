QT += network xml
QT -= gui

TARGET = qnapi
TEMPLATE = lib
CONFIG += staticlib \
          warn_on \
          release \
          silent \
          c++11

DESTDIR = $$PWD

#include(../deps/libmaia/maia.pri)

UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp

SOURCES += src/libqnapi.cpp \
           src/utils/synchttp.cpp

HEADERS += src/libqnapi.h \
           src/version.h \
           src/utils/synchttp.h


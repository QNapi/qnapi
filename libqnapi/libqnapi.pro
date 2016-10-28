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

SOURCES += libqnapi.cpp \
           src/utils/synchttp.cpp

HEADERS += libqnapi.h \
           src/utils/synchttp.h

CONFIG += warn_on \
 thread \
 qt \
 resources \
 release
TEMPLATE = app
SOURCES += src/main.cpp \
 src/napi.cpp \
 src/frmprogress.cpp \
 src/frmabout.cpp \
 src/frmoptions.cpp \
 src/frmupload.cpp \
 src/frmcorrect.cpp \
 src/frmreport.cpp \
 src/frmscan.cpp \
 src/frmcreateuser.cpp \
 src/qnapiconfig.cpp \
 src/qnapiapp.cpp \
 src/qdraglistwidget.cpp \
 src/movieinfo.cpp \
 src/multipartdata.cpp \
 src/qcumber/qinterprocesschannel.cpp \
 src/qcumber/qmanagedrequest.cpp \
 src/qcumber/qmanagedsocket.cpp \
 src/qcumber/qsingleapplication.cpp

HEADERS += src/napi.h \
 src/frmprogress.h \
 src/frmabout.h \
 src/frmoptions.h \
 src/frmupload.h \
 src/frmcorrect.h \
 src/frmreport.h \
 src/frmscan.h \
 src/frmcreateuser.h \
 src/version.h \
 src/qnapiconfig.h \
 src/qnapiapp.h \
 src/qdraglistwidget.h \
 src/movieinfo.h \
 src/multipartdata.h \
 src/napithread.h \
 src/synchttp.h \
 src/qcumber/qinterprocesschannel.h \
 src/qcumber/qmanagedrequest.h \
 src/qcumber/qmanagedsocket.h \
 src/qcumber/qsingleapplication.h \
 src/qcumber/qcumber.h


FORMS += ui/frmprogress.ui \
 ui/frmabout.ui \
 ui/frmoptions.ui \
 ui/frmupload.ui \
 ui/frmcorrect.ui \
 ui/frmreport.ui \
 ui/frmscan.ui \
 ui/frmcreateuser.ui
RESOURCES += src/resources.qrc
QT += network gui core

UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp

macx {
 QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
 CONFIG +=  x86  ppc
 ICON =  macx/qnapi.icns
 QMAKE_INFO_PLIST =  macx/Info_mac.plist
 TARGET =  QNapi
#7ZIP_BINARY.files = macx/7z macx/7z.so
#7ZIP_BINARY.path = Contents/Resources
#QMAKE_BUNDLE_DATA += 7ZIP_BINARY
}

win32 {
 RC_FILE = win32/qnapi.rc
}

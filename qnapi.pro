CONFIG += warn_on \
 thread \
 qt \
 resources \
 release
TEMPLATE = app
SOURCES += src/main.cpp \
 src/frmprogress.cpp \
 src/frmabout.cpp \
 src/frmoptions.cpp \
 src/frmupload.cpp \
 src/frmcorrect.cpp \
 src/frmreport.cpp \
 src/frmscan.cpp \
 src/frmcreateuser.cpp \
 src/frmsummary.cpp \
 src/qnapiconfig.cpp \
 src/qnapiapp.cpp \
 src/qdraglistwidget.cpp \
 src/movieinfo.cpp \
 src/qmultiparthttprequest.cpp \
 src/qcumber/qmanagedrequest.cpp \
 src/qcumber/qmanagedsocket.cpp \
 src/qcumber/qsingleapplication.cpp \
 src/qnapiabstractengine.cpp \
 src/qnapiprojektengine.cpp \
 src/qnapicli.cpp \
 src/qnapiopendialog.cpp \
 src/xmlrpc/client.cpp \
 src/xmlrpc/request.cpp \
 src/xmlrpc/variant.cpp \
 src/xmlrpc/response.cpp \
 src/qnapi.cpp \
 src/qopensubtitlesengine.cpp \
 src/frmnapiprojektconfig.cpp \
 src/frmlistsubtitles.cpp
HEADERS += src/frmprogress.h \
 src/frmabout.h \
 src/frmoptions.h \
 src/frmupload.h \
 src/frmcorrect.h \
 src/frmreport.h \
 src/frmscan.h \
 src/frmcreateuser.h \
 src/frmsummary.h \
 src/version.h \
 src/qnapiconfig.h \
 src/qnapiapp.h \
 src/qdraglistwidget.h \
 src/movieinfo.h \
 src/qmultiparthttprequest.h \
 src/qnapithread.h \
 src/synchttp.h \
 src/qcumber/qmanagedrequest.h \
 src/qcumber/qmanagedsocket.h \
 src/qcumber/qsingleapplication.h \
 src/qcumber/qcumber.h \
 src/qnapiabstractengine.h \
 src/qnapiprojektengine.h \
 src/qnapicli.h \
 src/qnapiopendialog.h \
 src/xmlrpc/qsyncxmlrpcclient.h \
 src/xmlrpc/variant.h \
 src/xmlrpc/client.h \
 src/xmlrpc/request.h \
 src/xmlrpc/response.h \
 src/qcheckedlistwidget.h \
 src/qnapi.h \
 src/qnapisubtitleinfo.h \
 src/qopensubtitlesengine.h \
 src/frmnapiprojektconfig.h \
 src/frmlistsubtitles.h
FORMS += ui/frmprogress.ui \
 ui/frmabout.ui \
 ui/frmoptions.ui \
 ui/frmupload.ui \
 ui/frmcorrect.ui \
 ui/frmreport.ui \
 ui/frmscan.ui \
 ui/frmcreateuser.ui \
 ui/frmsummary.ui \
 ui/napiprojekt/frmnapiprojektconfig.ui \
 ui/opensubtitles/frmopensubtitlesconfig.ui \
 ui/frmlistsubtitles.ui
RESOURCES += res/resources.qrc
QT += network gui core xml
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
OBJECTS_DIR = tmp
INCLUDEPATH = src
macx {
 QMAKE_MAC_SDK =  /Developer/SDKs/MacOSX10.4u.sdk
 CONFIG +=  x86  ppc
 ICON =  macx/qnapi.icns
 QMAKE_INFO_PLIST =  macx/Info.plist
 TARGET =  QNapi
 7ZIP_BINARY.files =  macx/7za
 7ZIP_BINARY.path =  Contents/Resources
 QMAKE_BUNDLE_DATA +=  7ZIP_BINARY
}
unix {
 INSTALL_PREFIX =  /usr
 target.path =  $${INSTALL_PREFIX}/bin
 doc.path =  $${INSTALL_PREFIX}/share/doc/$${TARGET}
 doc.files =  doc/ChangeLog \
  doc/changelog.gz \
  doc/README \
  doc/README.pl \
  doc/LICENSE \
  doc/LICENSE.pl \
  doc/COPYRIGHT \
  doc/qnapi-download.desktop \
  doc/qnapi-download.schemas
 man.path =  $${INSTALL_PREFIX}/share/man/man1
 man.files =  doc/$${TARGET}.1.gz
 icons.path =  /usr/share/icons
 icons.files =  res/qnapi.png  res/qnapi-48.png  res/qnapi-128.png  res/qnapi-512.png
 desktop.path =  /usr/share/applications
 desktop.files =  doc/$${TARGET}.desktop
 dolphin_integration.path =  /usr/share/apps/dolphin/servicemenus
 dolphin_integration.files =  doc/$${TARGET}-download.desktop
 d3lphin_integration.path =  /usr/share/apps/d3lphin/servicemenus
 d3lphin_integration.files =  doc/$${TARGET}-download.desktop
 konqueror_integration.path =  /usr/share/apps/konqueror/servicemenus
 konqueror_integration.files =  doc/$${TARGET}-download.desktop
 kde4_integration.path =  /usr/lib/kde4/share/kde4/services/ServiceMenus
 kde4_integration.files =  doc/$${TARGET}-download.desktop
 INSTALLS =  target \
  doc \
  man \
  icons \
  desktop \
  dolphin_integration \
  d3lphin_integration \
  konqueror_integration \
  kde4_integration
}
win32 {
 RC_FILE =  win32/qnapi.rc
 SOURCES +=  src/qcumber/qinterprocesschannel_win32.cpp
 HEADERS +=  src/qcumber/qinterprocesschannel_win32.h
}
!win32 {
 SOURCES +=  src/qcumber/qinterprocesschannel.cpp
 HEADERS +=  src/qcumber/qinterprocesschannel.h
}

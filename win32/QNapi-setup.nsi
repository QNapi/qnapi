; Define your application name
!define APPNAME "QNapi"
!define APPVER "0.1.8-snapshot"
!define APPNAMEANDVERSION "${APPNAME} ${APPVER}"

!include "MUI2.nsh"
!include "nsProcess.nsh"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "${APPNAME}-${APPVER}-setup.exe"

SetCompressor LZMA

!define MUI_ICON qnapi.ico
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\qnapi.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "QNapi" Section1

    ; Set Section properties
    SetOverwrite on

    ; Set Section Files and Shortcuts
    SetOutPath "$INSTDIR\"

    File /r "out\*"

    CreateDirectory "$SMPROGRAMS\${APPNAME}"
    CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\qnapi.exe"
    CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

Section -FinishSection

    WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteUninstaller "$INSTDIR\uninstall.exe"

    WriteRegStr HKCR ASFFile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR ASFFile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR AVIFile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR AVIFile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR divxfile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR divxfile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR mkvfile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR mkvfile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR mpegfile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR mpegfile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR mpgfile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR mpgfile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR RealPlayer.RA.6\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR RealPlayer.RA.6\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR RealPlayer.RAM.6\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR RealPlayer.RAM.6\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR RealPlayer.RM.6\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR RealPlayer.RM.6\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR RealPlayer.RMVB.6\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR RealPlayer.RMVB.6\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR rmvbfile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR rmvbfile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR WMP11.AssocFile.ASF\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR WMP11.AssocFile.ASF\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR WMP11.AssocFile.AVI\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR WMP11.AssocFile.AVI\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR WMP11.AssocFile.MPEG\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR WMP11.AssocFile.MPEG\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR WMP11.AssocFile.WMV\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR WMP11.AssocFile.WMV\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR WMVFile\shell\qnapi-pobierz "" 'Pobierz i dopasuj napisy do filmu z QNapi...'
    WriteRegStr HKCR WMVFile\shell\qnapi-pobierz\command "" '"$INSTDIR\qnapi.exe" "%1"'

SectionEnd

;Uninstall section
Section Uninstall

    ${nsProcess::KillProcess} "qnapi.exe" $R0
    ${nsProcess::Unload}

    DeleteRegKey HKCR ASFFile\shell\qnapi-pobierz
    DeleteRegKey HKCR AVIFile\shell\qnapi-pobierz
    DeleteRegKey HKCR divxfile\shell\qnapi-pobierz
    DeleteRegKey HKCR mkvfile\shell\qnapi-pobierz
    DeleteRegKey HKCR mpegfile\shell\qnapi-pobierz
    DeleteRegKey HKCR mpgfile\shell\qnapi-pobierz
    DeleteRegKey HKCR RealPlayer.RA.6\shell\qnapi-pobierz
    DeleteRegKey HKCR RealPlayer.RAM.6\shell\qnapi-pobierz
    DeleteRegKey HKCR RealPlayer.RM.6\shell\qnapi-pobierz
    DeleteRegKey HKCR RealPlayer.RMVB.6\shell\qnapi-pobierz
    DeleteRegKey HKCR rmvbfile\shell\qnapi-pobierz
    DeleteRegKey HKCR WMP11.AssocFile.ASF\shell\qnapi-pobierz
    DeleteRegKey HKCR WMP11.AssocFile.AVI\shell\qnapi-pobierz
    DeleteRegKey HKCR WMP11.AssocFile.MPEG\shell\qnapi-pobierz
    DeleteRegKey HKCR WMP11.AssocFile.WMV\shell\qnapi-pobierz
    DeleteRegKey HKCR WMVFile\shell\qnapi-pobierz

    ;Remove from registry...
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
    DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

    ; Delete self
    Delete "$INSTDIR\uninstall.exe"

    ; Delete Shortcuts
    Delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
    Delete "$SMPROGRAMS\${APPNAME}\Uninstall.lnk"

    ; Clean up test
    
    ; Remove remaining directories
    RMDir "$SMPROGRAMS\${APPNAME}"
    RMDir /r "$INSTDIR\"
    
SectionEnd

; eof
; Define your application name
!define APPNAME "QNapi"
!define APPVER "0.2.4-snapshot"
!define APPNAMEANDVERSION "${APPNAME} ${APPVER}"

!include "MUI2.nsh"
!include "nsProcess.nsh"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "${APPNAME}-${APPVER}-setup.exe"

SetCompressor LZMA

!define MUI_ICON qnapi-drive.ico
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

    ${nsProcess::KillProcess} "qnapi.exe" $R0
    ${nsProcess::Unload}

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

    ; clean up registry keys from qnapi <= 0.1.7
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


    WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteUninstaller "$INSTDIR\uninstall.exe"


    WriteRegStr HKCR SystemFileAssociations\.asf\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.asf\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.avi\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.avi\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.divx\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.divx\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.mkv\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.mkv\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.mov\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.mov\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.mp4\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.mp4\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.mpeg\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.mpeg\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.mpg\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.mpg\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.ogg\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.ogg\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.ogm\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.ogm\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.ogv\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.ogv\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.rm\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.rm\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.rmvb\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.rmvb\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'
    WriteRegStr HKCR SystemFileAssociations\.wmv\shell\qnapi "" 'Pobierz napisy z QNapi...'
    WriteRegStr HKCR SystemFileAssociations\.wmv\shell\qnapi\command "" '"$INSTDIR\qnapi.exe" "%1"'

SectionEnd

;Uninstall section
Section Uninstall

    ${nsProcess::KillProcess} "qnapi.exe" $R0
    ${nsProcess::Unload}

    DeleteRegKey HKCR SystemFileAssociations\.asf\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.avi\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.divx\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.mkv\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.mov\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.mp4\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.mpeg\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.mpg\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.ogg\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.ogm\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.ogv\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.rm\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.rmvb\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.wmv\shell\qnapi
    DeleteRegKey HKCR SystemFileAssociations\.asf\shell\qnapi


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

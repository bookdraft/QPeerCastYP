SetCompressor /FINAL /SOLID lzma

!define PRODUCT_NAME "QPeerCastYP"
!define PRODUCT_PUBLISHER "ciao"
!define PRODUCT_WEB_SITE "http://code.google.com/p/qpeercastyp/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\qpeercastyp.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible
!include "MUI.nsh"

; MUI Settings / Icons
; !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_ICON "images\qpeercastyp.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

; MUI Settings / Header
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"
 
; MUI Settings / Wizard
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

!insertmacro MUI_PAGE_WELCOME
; !define MUI_LICENSEPAGE_RADIOBUTTONS
; !insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN "$INSTDIR\qpeercastyp.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Japanese"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "qpeercastyp-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\QPeerCastYP"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "release\qpeercastyp.exe"
  File "README"
  File "ChangeLog"
  File "COPYING"
  CreateDirectory "$SMPROGRAMS\QPeerCastYP"
  CreateShortCut "$SMPROGRAMS\QPeerCastYP\QPeerCastYP.lnk" "$INSTDIR\qpeercastyp.exe"
  CreateShortCut "$DESKTOP\QPeerCastYP.lnk" "$INSTDIR\qpeercastyp.exe"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\QPeerCastYP\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\qpeercastyp.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\qpeercastyp.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(^Name) ÇçÌèúÇµÇƒÇ‡ÇÊÇÎÇµÇ¢Ç≈ÇµÇÂÇ§Ç©ÅH" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\README"
  Delete "$INSTDIR\COPYING"
  Delete "$INSTDIR\ChangeLog"
  Delete "$INSTDIR\qpeercastyp.exe"

  Delete "$SMPROGRAMS\QPeerCastYP\Uninstall.lnk"
  Delete "$SMPROGRAMS\QPeerCastYP\QPeerCastYP.lnk"
  Delete "$DESKTOP\QPeerCastYP.lnk"

  RMDir "$SMPROGRAMS\QPeerCastYP"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose false
SectionEnd

BrandingText "qpeercastyp.sourceforge.jp"

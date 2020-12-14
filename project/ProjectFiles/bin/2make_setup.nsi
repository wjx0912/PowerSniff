; 使用nsis-3.0b2-setup.exe
; 参考模板：gfx.nsi

;--------------------------------

!macro BIMAGE IMAGE PARMS
	Push $0
	GetTempFileName $0
	File /oname=$0 "${IMAGE}"
	SetBrandingImage ${PARMS} $0
	Delete $0
	Pop $0
!macroend

;--------------------------------

; The name of the installer
Name "PowerSniff"

; The file to write
OutFile "PowerSniff-Setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\PowerSniff

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\PowerSniff" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin
ShowInstDetails show
ShowUninstDetails show
XPStyle on
AddBrandingImage left 100
SetFont "Microsoft YaHei UI" 12
BrandingText " "

;--------------------------------

; Pages

Page license	wizardImage
Page components	wizardImage
Page directory	wizardImage
Page instfiles	wizardImage

LicenseText "PowerSniff License"
LicenseData "install\license.txt"

UninstPage uninstConfirm	un.wizardImage
UninstPage instfiles 		un.wizardImage

;--------------------------------

Section ""
	!insertmacro BIMAGE "install\wizard.bmp" /RESIZETOFIT
SectionEnd

Function wizardImage
	!insertmacro BIMAGE "install\wizard.bmp" /RESIZETOFIT
FunctionEnd

Function un.wizardImage
	!insertmacro BIMAGE "install\wizard.bmp" /RESIZETOFIT
FunctionEnd

;--------------------------------

; The stuff to install
Section "kernel module (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "PowerSniff.exe"
  File "PowerSniff.exe.manifest"
  SetOutPath "$INSTDIR\3rd"
  File /r "3rd\*"
  SetOutPath "$INSTDIR\data"
  ;File /r "data\*"
  SetOutPath "$INSTDIR\lib"
  File /r "lib\*"
  SetOutPath "$INSTDIR\plugin"
  File /r "plugin\*"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\PowerSniff "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\PowerSniff"
  CreateShortcut "$SMPROGRAMS\PowerSniff\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\PowerSniff\PowerSniff.lnk" "$INSTDIR\PowerSniff.exe" "" "$INSTDIR\PowerSniff.exe" 0
  
SectionEnd

Section "Desktop Shortcuts"
  CreateShortCut "$DESKTOP\PowerSniff.lnk" "$INSTDIR\PowerSniff.exe"
SectionEnd
;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PowerSniff"
  DeleteRegKey HKLM SOFTWARE\PowerSniff

  ; Remove files and uninstaller
  Delete $INSTDIR\PowerSniff.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\PowerSniff\*.*"
  Delete "$DESKTOP\PowerSniff.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\PowerSniff"
  RMDir /r "$INSTDIR"

SectionEnd

# Microsoft Developer Studio Project File - Name="capture" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=capture - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "capture.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "capture.mak" CFG="capture - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "capture - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "capture - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "capture"
# PROP Scc_LocalPath "..\widget"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "capture - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "samplecameraapp_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "$(BUITDIR)\widgets\inc" /I "..\..\framewidget\inc" /I "$(BUITDIR)\xmod" /I "$(BREWDIR)/inc" /D "NDEBUG" /D "AEE_SIMULATOR" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "samplecameraapp_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "capture - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "samplecameraapp_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(BUITDIR)\widgets\inc" /I "$(BUITDIR)\framewidget\inc" /I "$(BUITDIR)\xmod" /I "$(BREWDIR)/inc" /I "$(BREWDIR)/sdk/inc" /D "_DEBUG" /D "AEE_SIMULATOR" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "samplecameraapp_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"./capture.pdb" /debug /machine:I386 /out:"capture/capture.dll" /implib:"./capture.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=.\capture\capture.dll
TargetName=capture
InputPath=.\capture\capture.dll
SOURCE="$(InputPath)"

"$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) .

# End Custom Build

!ENDIF 

# Begin Target

# Name "capture - Win32 Release"
# Name "capture - Win32 Debug"
# Begin Group "App Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AEEBase.c
# End Source File
# Begin Source File

SOURCE=.\capture.c
# End Source File
# Begin Source File

SOURCE=.\scs.c
# End Source File
# Begin Source File

SOURCE=.\scs.h
# End Source File
# Begin Source File

SOURCE=.\widget_stubs.c
# End Source File
# Begin Source File

SOURCE=.\xmod\xmod.h
# End Source File
# Begin Source File

SOURCE=.\xmod\xmodimpl.c
# End Source File
# Begin Source File

SOURCE=.\xmod\xmodstub.c
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# End Target
# End Project

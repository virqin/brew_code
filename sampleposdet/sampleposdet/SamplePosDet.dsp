# Microsoft Developer Studio Project File - Name="SampleGPS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SampleGPS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SamplePosDet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SamplePosDet.mak" CFG="SampleGPS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SampleGPS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SampleGPS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "..\..\..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SampleGPS - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "$(BREWDIR)\inc" /I "$(BREWDIR)\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"sampleposdet.dll"

!ELSEIF  "$(CFG)" == "SampleGPS - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(BREWDIR)\inc" /I "$(BREWDIR)\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"sampleposdet.pdb" /debug /machine:I386 /out:"sampleposdet.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SampleGPS - Win32 Release"
# Name "SampleGPS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="$(BREWDIR)\sdk\src\AEEAppGen.c"
# End Source File
# Begin Source File

SOURCE="$(BREWDIR)\sdk\src\AEEModGen.c"
# End Source File
# Begin Source File

SOURCE=.\e_asin.c
# End Source File
# Begin Source File

SOURCE=.\SamplePosDet.c
# End Source File
# Begin Source File

SOURCE=.\SP_ConfigMenu.c
# End Source File
# Begin Source File

SOURCE=.\SP_GetGPSInfo.c
# End Source File
# Begin Source File

SOURCE=.\SP_MainMenu.c
# End Source File
# Begin Source File

SOURCE=.\SP_Track.c
# End Source File
# Begin Source File

SOURCE=.\transform.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SamplePosDet.h
# End Source File
# Begin Source File

SOURCE=.\sampleposdet_res.h
# End Source File
# Begin Source File

SOURCE=.\SP_Track.h
# End Source File
# Begin Source File

SOURCE=.\transform.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "brx;bmp;gif;jpg;jpeg"
# Begin Source File

SOURCE=.\sampleposdet.brx

!IF  "$(CFG)" == "SampleGPS - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\sampleposdet.brx
InputName=sampleposdet

BuildCmds= \
	$(BREWDIR)\sdk\tools\ResourceEditor\brewrc.exe -s -o $(InputDir)\$(InputName).bar -h $(InputDir)\$(InputPath)_res.h $(InputPath)

"$(InputDir)\$(InputName).bar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputPath)_res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "SampleGPS - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\sampleposdet.brx
InputName=sampleposdet

BuildCmds= \
	$(BREWDIR)\tools\ResourceEditor\brewrc.exe -s -o $(InputDir)\$(InputName).bar -h $(InputDir)\$(InputPath)_res.h $(InputPath)

"$(InputDir)\$(InputName).bar" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputPath)_res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project

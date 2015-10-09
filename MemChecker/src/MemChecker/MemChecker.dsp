# Microsoft Developer Studio Project File - Name="MemChecker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MemChecker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MemChecker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MemChecker.mak" CFG="MemChecker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MemChecker - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MemChecker - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MemChecker - Win32 Release"

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
# ADD CPP /nologo /MTd /W3 /GX /Zi /O2 /I "$(BREWDIR)\inc" /I "BXUI" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib user32.lib /nologo /subsystem:windows /dll /pdb:"MemChecker.pdb" /map /debug /machine:I386 /out:"MemChecker.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MemChecker - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(BREWDIR)\inc" /I "$(BREWDIR)\..\inc" /I "BXUI" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AEE_SIMULATOR" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "AEE_SIMULATOR" /MTd
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib user32.lib /nologo /subsystem:windows /dll /pdb:"MemChecker.pdb" /map /debug /machine:I386 /out:"MemChecker.dll" /pdbtype:sept
# SUBTRACT LINK32 /verbose /profile /pdb:none

!ENDIF 

# Begin Target

# Name "MemChecker - Win32 Release"
# Name "MemChecker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="$(BREWDIR)\src\AEEAppGen.c"
# End Source File
# Begin Source File

SOURCE="$(BREWDIR)\src\AEEModGen.c"
# End Source File
# Begin Source File

SOURCE=.\CallStackUtil.c
# End Source File
# Begin Source File

SOURCE=.\FuncHookUtil.c
# End Source File
# Begin Source File

SOURCE=.\IniFileParse.c
# End Source File
# Begin Source File

SOURCE=.\MemBlockList.c
# End Source File
# Begin Source File

SOURCE=.\MemChecker.c
# End Source File
# Begin Source File

SOURCE=.\MemCheckerLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\MemCheckerUtil.c
# End Source File
# Begin Source File

SOURCE=.\MemHook.c
# End Source File
# Begin Source File

SOURCE=.\StackHelper.c
# End Source File
# Begin Source File

SOURCE=.\SymbolMgr.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CallStackUtil.h
# End Source File
# Begin Source File

SOURCE=.\FuncHookUtil.h
# End Source File
# Begin Source File

SOURCE=.\IniFileParse.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\MemBlockList.h
# End Source File
# Begin Source File

SOURCE=.\MemCheckerUtil.h
# End Source File
# Begin Source File

SOURCE=.\MemHook.h
# End Source File
# Begin Source File

SOURCE=.\StackHelper.h
# End Source File
# Begin Source File

SOURCE=.\SymbolMgr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "BXUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BXUI\Base.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Base.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Button.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Button.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ClassFactory.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ClassFactory.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ClassList.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ClassList.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Container.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Container.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Control.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Control.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\DesktopWindow.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\DesktopWindow.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\EventDispatcher.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\EventDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IClassFactory.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IContainer.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IControlWrap.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\IControlWrap.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IDesktopWindow.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IEventDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IHandler.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ILayoutMgr.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IValueObject.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IVector.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IWidget.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\IWindow.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ListBox.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ListBox.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ListBox_IFace.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Menu.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Menu.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\MsgBox.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\MsgBox.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\SampleUI.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ScrollBar.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\StaticCtl.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\StaticCtl.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\StaticCtl_IFace.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\ValueObject.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\ValueObject.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Vector.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Vector.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\WidgetEvent.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\WidgetProp.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\Window.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\Window.h
# End Source File
# Begin Source File

SOURCE=.\BXUI\XUtil.c
# End Source File
# Begin Source File

SOURCE=.\BXUI\XUtil.h
# End Source File
# End Group
# End Target
# End Project

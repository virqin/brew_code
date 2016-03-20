set my_target=rscpooldemo

if not defined BREWSDK315EN goto ERR2
if not defined BREWADDINS goto ERR3

set BREWDIR=%BREWSDK315EN%
set BREWSDK=%BREWSDK315EN%
set brew_app_compile=%BREWADDINS%\common\bin\brewappcompile.exe
 
del *.o 
del *.elf
del *.mod

@if not exist %brew_app_compile% goto ERR

%brew_app_compile% %my_target%.mak

@pause
@exit

:ERR
echo ERROR brewappcompile.exe not found
@pause
@exit

:ERR2
echo Can't find BREW SDK 3.1.5
@pause
@exit

:ERR3
echo Can't find BREW ADDINS
@pause
@exit

 
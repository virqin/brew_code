set my_target=addrbookruim

REM BREW 2.1.x
rem set BREWDIR=%BREWSDK210EN%
rem set BREWSDK=%BREWSDK210EN%
rem set brew_app_compile=%BREWSDK210EN%\Bin\armcompile.exe

REM BREW 3.1.5
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
echo ERROR compile not found

@pause
@exit
 
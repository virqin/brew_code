@set AppName=MemChecker
@set PackageDir=%AppName%
@set ModDir=%PackageDir%\%AppName%

@if exist %PackageDir% rd /s /q %PackageDir%

@md %PackageDir%
copy Readme.txt %PackageDir%
copy ..\%AppName%.mif %PackageDir%

@md %ModDir%
copy %AppName%.dll %ModDir%
copy %AppName%.bar %ModDir%
copy config.ini %ModDir%

@pushd %PackageDir%
"%ProgramFiles%\7-Zip\7z.exe" a ..\%AppName%.zip
@popd

@rd /s /q %PackageDir%

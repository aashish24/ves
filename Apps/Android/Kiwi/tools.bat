@echo off

goto MAIN
::-----------------------------------------------
:: "%~f2" get abs path of %~2. 
::"%~fs2" get abs path with short names of %~2.
:setAbsPath
  setlocal
  set __absPath=%~f2
  endlocal && set %1=%__absPath%
  goto :eof
::-----------------------------------------------

:MAIN
for /f "tokens=*" %%a in ('where /F adb.exe') do ( set ADB=%%a )
echo/%%ADB%%=%ADB%

for /f "tokens=*" %%a in ('where /F android.bat') do ( set ANDROID=%%a )
echo/%%ANDROID%%=%ANDROID%

for /f "tokens=*" %%a in ('where /F ant.bat') do ( set ANT=%%a )
echo/%%ANT%%=%ANT%

for /f "tokens=*" %%a in ('where /F cmake.exe') do ( set CMAKE=%%a )
echo/%%CMAKE%%=%CMAKE%

set app_dir=%CD%
set build_dir=%app_dir%
call :setAbsPath source_dir %app_dir%/../../..
call :setAbsPath cmakeexternals %app_dir%/../CMakeBuild/build/CMakeExternals


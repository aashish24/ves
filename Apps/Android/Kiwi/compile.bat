@echo off

call ".\tools.bat"

cd %build_dir%
nmake

cd %app_dir%
%ANT% -buildfile %app_dir%/build.xml -Dbuilddir=%build_dir% debug

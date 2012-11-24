@echo off

call ".\tools.bat"

%ANDROID% update project --name KiwiViewer --path %app_dir% --target android-14

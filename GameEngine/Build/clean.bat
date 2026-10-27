@echo off
setlocal

echo Cleaning build folders...

rmdir /S /Q ..\x64\Debug
rmdir /S /Q ..\x64\Release
rmdir /S /Q ..\.vs

echo Clean completed.
pause
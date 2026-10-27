@echo off
setlocal

cd /d "%~dp0"

call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"

echo ================================
echo Building Debug...
echo ================================

set SOLUTION=..\..\GameEngine.slnx
set CONFIG=Debug
set PLATFORM=x64

msbuild "%SOLUTION%" /p:Configuration=%CONFIG% /p:Platform=%PLATFORM%

if %ERRORLEVEL% neq 0 (
    echo Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Copying Assets...
xcopy /E /I /Y ..\Assets ..\x64\Debug\Assets

echo.
echo Copying DLL files...

copy /Y ..\..\packages\SDL2Package.1.0.0\lib\native\x64\SDL2.dll ..\x64\Debug\
copy /Y ..\..\packages\SDL2Package.1.0.0\lib\native\x64\SDL2_image.dll ..\x64\Debug\
copy /Y ..\..\packages\SDL2Package.1.0.0\lib\native\x64\SDL2_ttf.dll ..\x64\Debug\
copy /Y ..\..\packages\SDL2Package.1.0.0\lib\native\x64\SDL2_mixer.dll ..\x64\Debug\

echo.
echo Debug build completed.
pause
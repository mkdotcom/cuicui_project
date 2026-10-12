@echo off
rem Builds the web version of the app into the "build" folder.
rem usage: _compile.bat debug   or   _compile.bat release
rem Needs emscripten (https://emscripten.org): set EMSDK_ENV in _setenv.bat if emcc is not in your PATH.

chcp 65001 >NUL
set PYTHONUTF8=1

if "%~1"=="" goto :invalid_parameter
if NOT "%1"=="debug" if NOT "%1"=="release" goto :invalid_parameter

call "%~dp0_setenv.bat"
cd /d "%~dp0"
echo building %1 version

if exist build rmdir /S /Q build
mkdir build
mkdir build\mkw_assets

set /p cuicui_core_cpp_files=<cuicui_core_cpp_files.txt
set /p your_app_cpp_files=<your_app_cpp_files.txt
set /p conf=<%1_conf.txt

call emcc -I../../ -std=c++14 -DCUICUI_WEB %your_app_cpp_files% %cuicui_core_cpp_files% %conf% -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s FETCH=1 -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS="['_main', '_resize_event', '_back_button_event', '_malloc', '_free']" -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'print', 'HEAPU8']" -o build\a.out.js
if errorlevel 1 goto :failed

copy /Y index.html build\ >NUL
copy /Y fav_icon_png.png build\ >NUL
copy /Y ..\..\cuicui_core\web\cuicui_core.css build\ >NUL
copy /Y ..\..\cuicui_core\web\cuicui_core.js build\ >NUL
xcopy /E /Y /Q ..\..\app\assets build\mkw_assets\ >NUL

echo done: open the "build" folder through a local web server, e.g. _serve.bat
goto :eof

:failed
echo BUILD FAILED
exit /b 1

:invalid_parameter
echo usage: _compile.bat debug   or   _compile.bat release
exit /b 1

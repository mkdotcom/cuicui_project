@echo off
rem Sets up emscripten for _compile.bat.
rem Change EMSDK_ENV to your emsdk installation (or remove this file's content if emcc is already in your PATH).
set EMSDK_ENV=c:\emsdk\emsdk_env.bat
if exist "%EMSDK_ENV%" call "%EMSDK_ENV%" >NUL 2>&1

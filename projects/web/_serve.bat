@echo off
rem Serves the "build" folder on http://localhost:8080 (needs Python 3).
cd /d "%~dp0build"
python -m http.server 8080

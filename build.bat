@echo off

mkdir build
pushd  build
cl -Zi ..\src\EscapeEngineWin32.cpp ..\src\glad.c -I..\include user32.lib gdi32.lib opengl32.lib
popd
pause
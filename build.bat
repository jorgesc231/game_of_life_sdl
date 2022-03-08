@echo off

cls

mkdir build
pushd build
REM cl -FC -Zi /FeGame_of_life ..\main.cpp /I..\include ..\lib\SDL2\SDL2.lib ..\lib\SDL2\SDL2main.lib ..\lib\SDL2\SDL2_ttf.lib Shell32.lib /link /SUBSYSTEM:WINDOWS
cl -O1 -FC /FeGame_of_life ..\main.cpp /I..\include ..\lib\SDL2\SDL2.lib ..\lib\SDL2\SDL2main.lib ..\lib\SDL2\SDL2_ttf.lib Shell32.lib /link /SUBSYSTEM:WINDOWS
popd
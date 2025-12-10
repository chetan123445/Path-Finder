@echo off
g++ src/path_finder_gui.cpp -o path_finder.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -std=c++17
if %errorlevel% == 0 (
    echo Compilation successful.
) else (
    echo Compilation failed.
)

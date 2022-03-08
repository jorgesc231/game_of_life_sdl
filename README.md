# Game of Life en SDL

Implementacion basica de Game of Life de John Conway usando SDL

### Requisitos

- Tener Instalado Visual Studio con las herramientas de desarrollo para C++
- Descargar las "Development Libraries" para Windows version "VC" de SDL2 de:

	https://libsdl.org/download-2.0.php

- Y con SDL2_ttf:

	https://github.com/libsdl-org/SDL_ttf/releases


- De la carpeta "lib/x64" del SDL2 descargado, copiar: SDL2.lib y SDL2main.lib a la carpeta "lib/SDL2" del proyecto
y tambien copiar "SDL2.dll" a la carpeta "build" de proyecto

- Y de la carpeta "include", copiar todo su contenido a la carpeta "include/SDL2" del proyecto

- Copiar el archivo "SDL2_ttf.lib" de la carpeta "lib/x64" de SDL2_ttf a la carpeta "lib/SDL2" del proyecto.

- Copiar el archivo "SDL2_ttf.dll" de la carpeta "lib/x64" de SDL2_ttf a la carpeta "build" del proyecto.

- Copiar el archivo "SDL_ttf.h" de la carpeta "include" de SDL2_ttf a la carpeta "include/SDL2" del proyecto


### Compilar

- Abrir "x64 Native Tools Command Prompt for VS 2019" que se debio haber instalado con Visual Studio
- Ir a la carpeta donde esta el proyecto y ejecutar

	build.bat

Esto deberia generar el archivo "Game_of_life.exe" en la carpeta build.
# Game of Life en SDL

Implementacion basica de Game of Life de John Conway usando SDL

## Jugar

Se puede probar aqui desde el navegador:

<https://jorgesc231.github.io/game_of_life_sdl/>


### Instrucciones de uso

- Click primario      = Agregar celda
- Click segundario    = Quitar celda

- Pausar simulacion   = Espacio
- Flecha hacia arriba = Aumenta la velocidad
- Flecha hacia abajo  = disminuir la velocidad
- Pausa + Enter       = Avanzar paso a paso
- Pausa + DELETE      = Limpiar


## Compilar

### Windows

1. Tener Instalado Visual Studio con las herramientas de desarrollo para C++
2. Descargar las "Development Libraries" para Windows version "VC" de SDL2 desde:

	https://libsdl.org/download-2.0.php

3. Y con SDL2_ttf:

	https://github.com/libsdl-org/SDL_ttf/releases


4. De la carpeta "lib/x64" del SDL2 descargado, copiar: SDL2.lib y SDL2main.lib a la carpeta "lib/SDL2" del proyecto
y tambien copiar "SDL2.dll" a la carpeta "build" de proyecto

5. Y de la carpeta "include", copiar todo su contenido a la carpeta "include/SDL2" del proyecto

6. Copiar el archivo "SDL2_ttf.lib" de la carpeta "lib/x64" de SDL2_ttf a la carpeta "lib/SDL2" del proyecto.

7. Copiar el archivo "SDL2_ttf.dll" de la carpeta "lib/x64" de SDL2_ttf a la carpeta "build" del proyecto.

8. Copiar el archivo "SDL_ttf.h" de la carpeta "include" de SDL2_ttf a la carpeta "include/SDL2" del proyecto


- Abrir "x64 Native Tools Command Prompt for VS 2019" que se debio haber instalado con Visual Studio

- Ir a la carpeta donde esta el proyecto y ejecutar:

	build.bat

Esto deberia generar el archivo "Game_of_life.exe" en la carpeta build.
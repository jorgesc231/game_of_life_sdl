#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define FPS 60
#define FRAME_TARGET_TIME (1000.0f / FPS)    // Cuando va a demorar en procesar un frame

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 800
#define CELDAS_X 50
#define CELDAS_Y 50

#ifndef __EMSCRIPTEN__
#define FONT_PATH "assets/OpenSans-Bold.ttf"
#else
#define FONT_PATH "build/assets/OpenSans-Bold.ttf"
#endif


// Estado de la pulsacion de una tecla
enum key_state
{
    still_released,
    just_pressed,
    just_released,
    still_pressed
};


struct game_state
{
    int pausa ;
    int avanzar;
    int limpiar;
    
    long int count;
    
    float tiempo_total;
    float update_step;
    
    
	// Dimensiones de la celda
	int celda_width;
	int celda_height;
} state;


void mostrar_pausa();
void draw_game_state(struct game_state *state);
void draw_label (char *string, int x, int y, TTF_Font *font, SDL_Color color, bool fast = false);


SDL_Color GRAY  = { 155, 155, 155, 255 };
SDL_Color WHITE = { 255, 255, 255, 255 };
SDL_Color RED   = { 255,   0,   0, 255 };


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;

// Renderizado de texto
TTF_Font *font = NULL;
TTF_Font *normal_font = NULL;
SDL_Texture *text_texture = NULL;
int text_w = 0;
int text_h = 0;

int prev_time = 0;
int keycount = 0;

bool game_is_running = false;
bool show_game_state = false;
bool should_reinit = false;

int suma = 0;

SDL_Rect celda = {0, 0, state.celda_width, state.celda_height};
	
// Estado de las celdas. Vivas = 1; Muertas = 0;
bool estado[CELDAS_X][CELDAS_Y] = { 0 };
bool estado_copia[CELDAS_X][CELDAS_Y] = { 0 };

// Keyboard state
Uint8 *last_state;
const Uint8 *current_state;

// Mouse state
int rb_mouse_down = 0;
int lb_mouse_down = 0;
int mouse_x = 0;
int mouse_y = 0;

int selected_cell_x = 0;
int selected_cell_y = 0;

void main_loop();

bool return_down = false;
bool space_down = false;
bool up_down = false;
bool down_down = false;
bool backspace_down = false;
bool f1_down = false;
bool return_pressed = false;
bool space_pressed = false;
bool up_pressed = false;
bool down_pressed = false;
bool backspace_pressed = false;
bool f1_pressed = false;


int get_key_state(SDL_Keycode key_code)
{
#ifdef __EMSCRIPTEN__
    if (key_code == SDL_SCANCODE_RETURN && return_pressed)
        return just_pressed;
    if (key_code == SDL_SCANCODE_SPACE && space_pressed)
        return just_pressed;
    if (key_code == SDL_SCANCODE_UP && up_pressed)
        return just_pressed;
    if (key_code == SDL_SCANCODE_DOWN && down_pressed)
        return just_pressed;
    if (key_code == SDL_SCANCODE_BACKSPACE && backspace_pressed)
        return just_pressed;
    if (key_code == SDL_SCANCODE_F1 && f1_pressed)
        return just_pressed;

    else return still_released;
#else
    if (last_state[key_code])
    {
        if (current_state[key_code]) return still_pressed;
        else return just_released;
    } else {
        if (current_state[key_code]) return just_pressed;
        else return still_released;
    }
#endif
}

// Funcion para calcular el modulo
// En C usar modulo con numeros negativos da negativo, en python no.
int mod (int divisor, int dividendo)
{
	int residuo = divisor % dividendo;
	
	return residuo < 0 ? residuo + dividendo : residuo;
}


int main(int argc, char *argv[])
{
	// Crea la ventana
	window = SDL_CreateWindow("Juego de la Vida", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              window_width, window_height, 0);
	
    // Permite redimensionar la ventana
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowMinimumSize(window, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
    
	// Crear el renderer
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
       
	if (window && renderer)  game_is_running = true;
	else return -1;

    // Initialize SDL_ttf and load fonts
    if (TTF_Init() == -1) return -1;

    font = TTF_OpenFont(FONT_PATH, 32);
    if (!font)
    {
        printf("Error al cargar la fuente 32\n%s\n", TTF_GetError());
        return -1;
    }

    normal_font = TTF_OpenFont(FONT_PATH, 16);
    if (!normal_font)
    {
        printf("Error al cargar la fuente 16\n%s\n", TTF_GetError());
        return -1;
    }
    

    
    state.pausa = 0;
    state.avanzar = 0;
    state.limpiar = 0;
    
    state.tiempo_total = 0.0f;
    state.update_step = 0.1f;
    
    // Dimensiones de la celda
    state.celda_width = window_width / CELDAS_X;
    state.celda_height = window_height / CELDAS_Y;

    celda = { 0, 0, state.celda_width, state.celda_height };
	  
    
	// Automata palo
	estado[5][3] = 1;
	estado[5][4] = 1;
	estado[5][5] = 1;
	
	// Automata movil
	estado[21][21] = 1;
	estado[22][22] = 1;
	estado[22][23] = 1;
	estado[21][23] = 1;
	estado[20][23] = 1;
    
    
    prev_time = SDL_GetTicks();
    keycount = 0;
    
    if (game_is_running)
    {
#ifndef __EMSCRIPTEN__
        main_loop();
#endif

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop, 0, 0);
#endif
    }
   
	return 0;
}

void main_loop()
{
#ifndef __EMSCRIPTEN__
    while (game_is_running)
    {
#endif
        // Actualiza el estado del teclado
        if (last_state == NULL && current_state == NULL)
        {
            current_state = SDL_GetKeyboardState(&keycount);
            last_state = (Uint8 *) malloc(sizeof(Uint8) * keycount);
            
            int counter = 0;
            for (; counter <= keycount; counter++)
                last_state[counter] = 0;
        } else {
            // last_state = current_state
            for (int counter = 0; counter <= keycount; counter++)
                last_state[counter] = current_state[counter];
        }

        
        SDL_Event event;
        
        // Procesa los eventos
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    game_is_running = false;
                } break;

                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                        return_down = true;

                    if (event.key.keysym.sym == SDLK_SPACE)
                        space_down = true;

                    if (event.key.keysym.sym == SDLK_UP)
                        up_down = true;

                    if (event.key.keysym.sym == SDLK_DOWN)
                        down_down = true;

                    if (event.key.keysym.sym == SDLK_BACKSPACE)
                        backspace_down = true;

                    if (event.key.keysym.sym == SDLK_F1)
                        f1_down = true;
                
                } break;

                case SDL_KEYUP:
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        return_down = false;
                        return_pressed = true;
                    }

                    if (event.key.keysym.sym == SDLK_SPACE)
                    {
                        space_down = false;
                        space_pressed = true;
                    }

                    if (event.key.keysym.sym == SDLK_UP)
                    {
                        up_down = false;
                        up_pressed = true;
                    }

                    if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        down_down = false;
                        down_pressed = true;
                    }

                    if (event.key.keysym.sym == SDLK_BACKSPACE)
                    {
                        backspace_down = false;
                        backspace_pressed = true;
                    }

                    if (event.key.keysym.sym == SDLK_F1)
                    {
                        f1_down = false;
                        f1_pressed = true;
                    }

                } break;
                
                case SDL_MOUSEMOTION:
                {
                    // Update mouse position
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                } break;
                
                case SDL_MOUSEBUTTONDOWN:
                {
                    // update button down state if left-clicking
                    if (event.button.button == SDL_BUTTON_LEFT) lb_mouse_down = 1;
                    if (event.button.button == SDL_BUTTON_RIGHT) rb_mouse_down = 1;
                } break;
                
                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button == SDL_BUTTON_LEFT) lb_mouse_down = 0;
                    if (event.button.button == SDL_BUTTON_RIGHT) rb_mouse_down = 0;
                } break;
                
                case SDL_WINDOWEVENT:
                {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        // No se la diferencia con el de abajo, por ahora lo dejo asi
                        ;
                    }
                    
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        should_reinit = (event.window.data1 != window_width) || (event.window.data2 != window_height);
                        
                        if (should_reinit)
                        {
                            // Almacena el nuevo tamano de pantalla
                            window_width = event.window.data1;
                            window_height = event.window.data2;
                            
                            // Recalcula el tamano de las celdas
                            state.celda_width = window_width / CELDAS_X;
                            state.celda_height = window_height / CELDAS_Y;
                            
                            celda = {0, 0, state.celda_width, state.celda_height};
                            
                            
                            // TODO: Deberia adaptar el tamano de la fuente?
                            // TODO: Mantener el aspect ratio?
                        }
                    }
                } break;
            }
        }
        
        
        // Bloqueo de FPS (no muy preciso)
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - prev_time);
		
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        {
            // TODO: Esto no es muy preciso...
            SDL_Delay (time_to_wait);
        }
		
        int current_time = SDL_GetTicks();
        float delta_time = (float)(current_time - prev_time) / 1000.0f; // elapsed time
        prev_time = current_time;
		
        
        if (get_key_state(SDL_SCANCODE_SPACE) == just_pressed) state.pausa = !state.pausa;
        
        state.avanzar = get_key_state(SDL_SCANCODE_RETURN) == just_pressed;
        
        
        // Modifica la velocidad de actualizacion del estado
        if (state.update_step <= 0.1f)
        {
            if (get_key_state(SDL_SCANCODE_UP)   == just_pressed) state.update_step -= 0.005f;
            if (get_key_state(SDL_SCANCODE_DOWN) == just_pressed) state.update_step += 0.005f;
        }
        if (state.update_step > 0.1f) 
        {
            if (get_key_state(SDL_SCANCODE_UP)   == just_pressed) state.update_step -= 0.05f;
            if (get_key_state(SDL_SCANCODE_DOWN) == just_pressed) state.update_step += 0.05f;
        }
        
        // El tiempo de actualizacion no sea mayor a 1 seg o menor al tiempo en que se demora en generar // un frame: 0.0166
        if (state.update_step >= 1.0f) state.update_step = 1.0f;
        if (state.update_step <= 0.02f) state.update_step = 0.02f;
        
        // Copia del estado actual del juego
        for (int x = 0; x < CELDAS_X; x++)
        {
            for (int y = 0; y < CELDAS_Y; y++)
            {
                estado_copia[x][y] = estado[x][y];
                if (state.limpiar) estado_copia[x][y] = 0;
            }
        }
        
        // Para que no siga subiendo por siempre y termine por desbordarse
        if (state.tiempo_total >= 30.0f) state.tiempo_total = 0.0f;
        
        state.tiempo_total += delta_time;
        
        // Actualiza el estado del juego
        if ((!state.pausa || (state.pausa && state.avanzar)) && state.tiempo_total >= state.update_step) 
        {
            state.tiempo_total = 0.0f;
            
            state.count++;
            
            for (int x = 0; x < CELDAS_X; x++)
            {
                for (int y = 0; y < CELDAS_Y; y++)
                {
                    // Calculamos el numero de vecinos cercanos
                    suma = estado[mod((x - 1), CELDAS_X)][mod((y - 1), CELDAS_Y)] + 
                        estado[mod(x, CELDAS_X)][mod(y - 1, CELDAS_Y)] + 
                        estado[mod(x + 1, CELDAS_X)][mod(y - 1, CELDAS_Y)] + 
                        estado[mod(x - 1, CELDAS_X)][mod(y, CELDAS_Y)] + 
                        estado[mod(x + 1, CELDAS_X)][mod(y, CELDAS_Y)] + 
                        estado[mod(x - 1, CELDAS_X)][mod(y + 1, CELDAS_Y)] + 
                        estado[mod(x, CELDAS_X)][mod(y + 1, CELDAS_Y)] + 
                        estado[mod(x + 1, CELDAS_X)][mod(y + 1, CELDAS_Y)];
					
					
                    // Regla 1: Una celda muerta con exactamente 3 vecinas vivas, "revive".
                    if (estado[x][y] == 0 && suma == 3) 
                        estado_copia[x][y] = 1;
					
                    // Regla 2: Una celda viva con menos de 2 o mas de 3 vecinas vivas, "muere".
                    if (estado[x][y] == 1 && (suma < 2 || suma > 3)) 
                        estado_copia[x][y] = 0;
                }
            }
        }
        
        // Render
        SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
		
        // Empieza a dibujar
        
        // Dibuja el estado cada fotograma
        for (int x = 0; x < CELDAS_X; x++)
        {
            for (int y = 0; y < CELDAS_Y; y++)
            {
                celda.x = state.celda_width * x;
                celda.y = state.celda_height * y;
				
                // Dibujado de la celda para cada par de x e y
                if (estado_copia[x][y] == 0)
                {
                    SDL_SetRenderDrawColor (renderer, 155, 155, 155, 255);
                    SDL_RenderDrawRect(renderer, &celda);
                    
                    // Optimizar usando:
                    // int SDL_RenderDrawRects(SDL_Renderer * renderer, const SDL_Rect * rects, int count);
                } else {
                    SDL_SetRenderDrawColor (renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &celda);
                    
                    // Optimizar usando:
                    // int SDL_RenderFillRects(SDL_Renderer * renderer, const SDL_Rect * rects, int count);
                }
            }
        }
        
        if (state.pausa)
        {
            if (state.limpiar = get_key_state(SDL_SCANCODE_BACKSPACE) == just_pressed)
                state.count = 0;
            
            mostrar_pausa();
        }
        
        
        if (get_key_state(SDL_SCANCODE_F1) == just_pressed) show_game_state = !show_game_state;
        
        if (show_game_state)
            draw_game_state(&state);
        
        
        
        
        
        // Copia del estado actual del juego
        for (int x = 0; x < CELDAS_X; x++)
        {
            for (int y = 0; y < CELDAS_Y; y++)
                estado[x][y] = estado_copia[x][y];
        }
        
        
        if (lb_mouse_down)
        {
            selected_cell_x = mouse_x / state.celda_width;
            selected_cell_y = mouse_y / state.celda_height;
            
            estado[selected_cell_x][selected_cell_y] = 1;
        }
        
        if (rb_mouse_down)
        {
            selected_cell_x = mouse_x / state.celda_width;
            selected_cell_y = mouse_y / state.celda_height;
            
            estado[selected_cell_x][selected_cell_y] = 0;
        }
        
        if (get_key_state(SDL_SCANCODE_ESCAPE) == just_pressed) game_is_running = false;
        

        return_pressed = false;
        space_pressed = false;
        up_pressed = false;
        down_pressed = false;
        backspace_pressed = false;
        f1_pressed = false;
        
        // Termina de dibujar
        SDL_RenderPresent(renderer);
    
#ifndef __EMSCRIPTEN__
    }
#endif
}


// Mostrar texto en pantalla
void draw_label (char *string, int x, int y, TTF_Font *font, SDL_Color color, bool fast)
{
    SDL_Surface *text_surface = NULL;
    SDL_Rect rect;
    
    if (text_texture != NULL)
    {
        SDL_DestroyTexture(text_texture);
    }
    
    // Si no hay texto
    TTF_SizeText(font, string, &text_w, &text_h);
    
    if (text_w == 0)
        string = " ";
    
    // Para mostrar texto menos importante mas rapido 
    if (!fast)
        text_surface = TTF_RenderText_Blended (font, string, color);
    else
        text_surface = TTF_RenderText_Solid(font, string, color);
    
    if (text_surface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf - Error: %s\n", TTF_GetError());
        ;
    } else {
        // Create texture from surface pixels
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        
        if (text_texture == NULL)
        {
            printf ("Unable to create texture from renderer text! - Error: %s\n", SDL_GetError());
            ;
        } else {
            // Get image dimensions
            rect.w = text_surface->w;
            rect.h = text_surface->h;
            
            rect.x = x;
            rect.y = y;
        }
        
        // Get rid of old surface
        SDL_FreeSurface(text_surface);
    }
    
    // Render font texture to screen
    SDL_RenderCopy(renderer, text_texture, NULL, &rect);
}



void mostrar_pausa()
{
    //SDL_Rect rect = {window_width / 4, window_height / 4, window_width / 2, window_height / 2};
    SDL_Rect rect = {0, 0, window_width, window_height};
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    SDL_SetRenderDrawColor (renderer, 0, 0, 0, 152);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    //SDL_SetRenderDrawColor (renderer, 255, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &rect);
    
    
    char *pausa_text = "PAUSA!!!";
    TTF_SizeText(font, pausa_text, &text_w, &text_h);
    draw_label(pausa_text, 
               rect.x + rect.w * 0.5 - text_w * 0.5, rect.y + rect.h * 0.3 - text_h * 0.5, font, WHITE);
    
    char *aumentar_text = "FLECHA HACIA ARRIBA PARA AUMENTAR LA VELOCIDAD";
    TTF_SizeText(normal_font, aumentar_text, &text_w, &text_h);
    draw_label(aumentar_text, 
               window_width * 0.5 - text_w * 0.5, window_height * 0.5 - text_h * 0.5, 
               normal_font, WHITE);
    
    char *disminuir_text = "FLECHA HACIA ABAJO PARA DISMINUIR LA VELOCIDAD";
    TTF_SizeText(normal_font, disminuir_text, &text_w, &text_h);
    draw_label(disminuir_text, 
               window_width * 0.5 - text_w * 0.5, window_height * 0.5 - text_h * 0.5 + text_h, 
               normal_font, WHITE);
    
    char *paso_text = "ENTER PARA AVANZAR PASO A PASO";
    TTF_SizeText(normal_font, paso_text, &text_w, &text_h);
    draw_label(paso_text, 
               window_width * 0.5 - text_w * 0.5, window_height * 0.5 - text_h * 0.5 + text_h * 2, 
               normal_font, WHITE);
    
    char *delete_text = "DELETE PARA LIMPIAR";
    TTF_SizeText(normal_font, delete_text, &text_w, &text_h);
    draw_label(delete_text, 
               window_width * 0.5 - text_w * 0.5, window_height * 0.5 - text_h * 0.5 + text_h * 3, 
               normal_font, WHITE);
    
    char *salir_text = "ESCAPE PARA SALIR";
    TTF_SizeText(normal_font, salir_text, &text_w, &text_h);
    draw_label(salir_text, 
               window_width * 0.5 - text_w * 0.5, window_height * 0.5 - text_h * 0.5 + text_h * 4, 
               normal_font, WHITE);
}


void draw_game_state(struct game_state *state)
{
    int text_w = 0, text_h = 0;
    
    TTF_SizeText(normal_font, "A", &text_w, &text_h);
    
    char buffer[128];
    
    // TODO: Calcular esto de verdad...
    snprintf(buffer, 128, "ms: %f - FPS: %d", FRAME_TARGET_TIME, FPS);
    draw_label(buffer, 0, 0, normal_font, RED, true);
    
    snprintf(buffer, 128, "Time to Update: %f", state->update_step);
    draw_label(buffer, 0, text_h, normal_font, RED);
    
    snprintf(buffer, 128, "Elapsed Time: %f", state->tiempo_total);
    draw_label(buffer, 0, text_h * 2, normal_font, RED, true);
    
    
    // Tamano de las celdas
    snprintf(buffer, 128, "Cell size: %d x %d", state->celda_width, state->celda_height);
    draw_label(buffer, 0, text_h * 3, normal_font, RED, true);
    
    // Cantidad de celdas
    snprintf(buffer, 128, "Cells: %d x %d", CELDAS_X, CELDAS_Y);
    draw_label(buffer, 0, text_h * 4, normal_font, RED, true);
    
    
    // Tamano de la ventana
    snprintf(buffer, 128, "Res: %d x %d", window_width, window_height);
    draw_label(buffer, 0, text_h * 5, normal_font, RED, true);
    
    
    // Cantidad de cambios
    snprintf(buffer, 128, "Ciclos: %d", state->count);
    draw_label(buffer, 0, text_h * 6, normal_font, RED, true);
}
//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>

#include "invaders.h"
#include "board.h"
#include "engine.h"

//global surface pointers, initialized to NULL
SDL_Surface *g_surfaces[SURFACE_DEPTH] = {};





int main(int ac, char *av[]) {
    if ( initialize_board() == -1 ) {
        return 1;
    }
    game_loop();
    return 0;
}

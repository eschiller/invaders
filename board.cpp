/*
 * board.cpp
 *
 *  Created on: May 28, 2014
 *      Author: eschiller
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

#include "board.h"

using namespace std;

//function prototypes
int initialize_sprite_textures();


//globals
SDL_Window *g_mainwin = NULL;
SDL_Surface *g_window_surface = NULL;
SDL_Renderer *g_renderer = NULL;

/*
 * structure to handle the details for any individual sprite
 */
struct sprite {
    int key;
    string image;
    SDL_Texture *tex;
    int width;
    int height;
    int xpos;
    int ypos;
    int xvel;
    int yvel;
    int frames_per_move;
    int curr_frame;
    int render;
};


/*
 * Details regarding the sprites in invaders. Individual sprite data will be
 * adjusted by externally available functions.
 */
static struct sprite all_sprites[] = {
        { BACKGROUND, "images/space.png", NULL, 640, 640, 0, 0, 0, 0, 40, 0, 1 },
        { SHIP, "images/ship.png", NULL, 48, 48, ((WIN_WIDTH / 2) - 24), (WIN_HEIGHT - 48), 0, 0, 1, 0, 1 },
        { SHOT, "images/shot.png", NULL, 4, 8, 320, 460, 0, -16, 1, 0, 0 },
        { SHOT2, "images/shot.png", NULL, 4, 8, 320, 460, 0, -16, 1, 0, 0 },
        { SHOT3, "images/shot.png", NULL, 4, 8, 320, 460, 0, -16, 1, 0, 0 },
        { ALIEN_1, "images/alien.png", NULL, 64, 64, 160, 80, 0, 0, 1, 0, 1},
        { ALIEN_2, "images/alien.png", NULL, 64, 64, 480, 80, 0, 0, 1, 0, 1},
        { END_OF_ARRAY, "", NULL, 0, 0, 0, 0, 0, 0, 0 },
};



/*
 * initialize_sprite_textures loads an appropriate *SDL_Texure for each sprite
 * int the all_sprites array
 */
int initialize_sprite_textures() {
    int i;
    SDL_Surface *tempsurface;
    int total_textures_loaded = 0;

    /*
     * iterate through the array loading images to surfaces, then converting
     * surfaces to textures
     */
    while (all_sprites[i].key != -1) {
        if((tempsurface = IMG_Load(all_sprites[i].image.c_str())) == NULL) {
            fprintf(stderr, "Invaders: Unable to load image %s\n%s\n",
                    all_sprites[i].image.c_str(), IMG_GetError());
            return -1;
        }
        if((all_sprites[i].tex = SDL_CreateTextureFromSurface(g_renderer, tempsurface)) == NULL) {
            fprintf(stderr, "Invaders: %s\n", SDL_GetError());
            return -1;
        }
        printf("loaded a texture for %s\n", all_sprites[i].image.c_str());
        i++;
    }
    SDL_FreeSurface(tempsurface);
    return total_textures_loaded;
}


/*
 * initialize_board does all preloading of SDL and assets needed before
 * entering our main loop
 *
 * returns 0 if successful, -1 on error
 */
int initialize_board() {
    //initialize SDL and the main window
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("Invaders: SDL_Init %s\n", SDL_GetError());
        return -1;
    } else {
        if ((g_mainwin = SDL_CreateWindow( "Invaders", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL) {
            printf("Invaders: SDL_CreateWindow: %s\n", SDL_GetError());
        }
    }

    IMG_Init( IMG_INIT_PNG );

    //create the renderer
    if ((g_renderer = SDL_CreateRenderer(g_mainwin, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
        fprintf(stderr, "Invaders: SDL_CreateRenderer: %s\n", SDL_GetError());
    } else {
        SDL_SetRenderDrawColor( g_renderer, 0x00, 0x00, 0x00, 0x00 );
    }

    if (initialize_sprite_textures() == -1) {
        return -1;
    }
    return 0;
}


/*
 * cleanup destroys and cleans up any resources that don't automatically
 * de-allocate memory
 */
int cleanup() {
    int i = 0;
    while (all_sprites[i].key != END_OF_ARRAY) {
        SDL_DestroyTexture(all_sprites[i].tex);
    }
    SDL_DestroyRenderer( g_renderer );
    SDL_DestroyWindow( g_mainwin );
    IMG_Quit();
    SDL_Quit();
    return 0;
}


/*
 * render_all steps through the array and renders each texture to the window.
 */
int render_all() {
    int i = 0;
    SDL_RenderClear(g_renderer);
    SDL_Rect dest_rect;
    while ( all_sprites[i].key != END_OF_ARRAY) {
        if (all_sprites[i].render == 1) {
            dest_rect.x = all_sprites[i].xpos;
            dest_rect.y = all_sprites[i].ypos;
            dest_rect.w = all_sprites[i].width;
            dest_rect.h = all_sprites[i].height;
            SDL_RenderCopy( g_renderer, all_sprites[i].tex, NULL, &dest_rect);
        }
        i++;
    }
    SDL_RenderPresent( g_renderer );
    return 0;
}


/*
 * get_sprite_x is a simple getter function.
 *
 * returns the xpos of the passed sprite.
 */
int get_sprite_x(int sprite_index) {
    return all_sprites[sprite_index].xpos;
}


/*
 * get_sprite_y is a simple getter function.
 *
 * returns the ypos of the passed sprite.
 */
int get_sprite_y(int sprite_index) {
    return all_sprites[sprite_index].ypos;
}


/*
 * get_sprite_width is a simple getter function.
 *
 * returns the width of the passed sprite
 */
int get_sprite_width(int sprite_index) {
    return all_sprites[sprite_index].width;
}


/*
 * get_sprite_height is a simple getter function.
 *
 * returns the height of the passed sprite
 */
int get_sprite_height(int sprite_index) {
    return all_sprites[sprite_index].height;
}


/*
 * enable_sprite sets the value of "render" to 1, causing the sprite to become
 * rendered on the next frame, until disable_sprite is called.
 *
 */
void enable_sprite(int sprite_index) {
    all_sprites[sprite_index].render = 1;
}


/*
 * enable_sprite sets the value of "render" to 0, causing the sprite to no
 * longer be rendered.
 *
 */
void disable_sprite(int sprite_index) {
    all_sprites[sprite_index].render = 0;
}


/*
 * move_all_sprites adjusts xpos and y pos for a sprite by the integers
 * indicated in xvel and yvel
 *
 */
int move_all_sprites() {
    int i = 0;
    while(all_sprites[i].key != END_OF_ARRAY) {
        move_sprite(i, all_sprites[i].xvel, all_sprites[i].yvel);
        i++;
    }
    return 0;
}


/*
 * move_sprite takes the sprite indicated by sprite index, and moves its x and
 * y cooridantes in the all_sprites array by the numbers indicated by x_delta
 * and y_delta.
 *
 * returns 0 on success, -1 if we hit a wall.
 */
int move_sprite(int sprite_index, int x_delta, int y_delta) {
    int newx, newy;
    newx = x_delta + all_sprites[sprite_index].xpos;
    newy = y_delta + all_sprites[sprite_index].ypos;
    int rval = 0;

    //check the bounds of the gameplay walls and bring back inside if needed
    if (newx < 0) {
        newx = 0;
        rval = -1;
    } else if (newx > WIN_WIDTH) {
        newx = WIN_WIDTH;
        rval = -1;
    }

    if (newy < -8) {
        newy = -8;
        rval = -1;
    } else if (newy > WIN_HEIGHT) {
        newy = WIN_HEIGHT;
        rval = -1;
    }

    //set the x and y in array
    all_sprites[sprite_index].xpos = newx;
    all_sprites[sprite_index].ypos = newy;
    return rval;
}


/*
 * set_sprite_x sets the x position of the passed sprite to the passed new x
 * position. If the x position is outside the bounds of the window, x is set to
 * the closest bounding edge.
 *
 * returns the new x position.
 */
int set_sprite_x(int sprite_index, int newx) {
    all_sprites[sprite_index].xpos = newx;
    return newx;
}



/*
 * set_sprite_y sets the y position of the passed sprite to the passed new y
 * position. If the y position is outside the bounds of the window, y is set to
 * the closest bounding edge.
 *
 * returns the new y position.
 */
int set_sprite_y(int sprite_index, int newy) {
    all_sprites[sprite_index].ypos = newy;
    return newy;
}


/*
 * set_sprite_pos is a convenience wrapper around set_sprite_x and set_sprite_y
 *
 * returns 0
 */
int set_sprite_pos(int sprite_index, int newx, int newy) {
    set_sprite_x(sprite_index, newx);
    set_sprite_y(sprite_index, newy);
    return 0;
}


int is_enabled(int sprite_index) {
    return all_sprites[sprite_index].render;
}


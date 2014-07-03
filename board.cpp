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
#include <iostream>
#include <vector>

#include "board.h"
#include "sprite.h"
#include "engine.h"
#include "animation.h"

using namespace std;


//globals
SDL_Window *g_mainwin = NULL;
SDL_Surface *g_window_surface = NULL;
SDL_Renderer *g_renderer = NULL;

//function declarations
int ship_shoot();
int check_all_collisions();

//data structure delcarations
static Sprite sprites[20];
static Animation animations[20];

/*
 * initialize_sprites initializes all used sprites in the sprites array. This
 * sets the starting configuration for all images on the screen.
 *
 * It's worth noting that when rendering, the sprites are layered on top of
 * one another, starting with index 0, then iterating through the final
 * active sprite. This means a sprite with lower index will be overlapped by a
 * sprite with higher index.
 *
 * The final sprite in the array must have an index of "end_of_array", marking
 * it as the terminal sprite. This is used to prevent hitting uninitialized
 * sprites when iterating through the array.
 *
 * returns 0 on success
 */
int initialize_sprites() {
    sprites[0] = Sprite("background", "images/space.png", WIN_WIDTH, WIN_HEIGHT, 0, 0, 1);
    sprites[1] = Sprite("ship", "images/ship_stop.png", 40, 40, ((WIN_WIDTH / 2) - 20), (WIN_HEIGHT - 50), 1, animations[0]);
    sprites[2] = Sprite("shot_1", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[3] = Sprite("shot_2", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[4] = Sprite("shot_3", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[5] = Sprite("shot_4", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[6] = Sprite("shot_5", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[7] = Sprite("shot_6", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[8] = Sprite("shot_7", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[9] = Sprite("shot_8", "images/shot.png", 4, 8, 320, 460, 0);
    sprites[10] = Sprite("alien_1", "images/alien.png", 64, 64, 68, 80, 1);
    sprites[11] = Sprite("alien_2", "images/alien.png", 64, 64, 268, 80, 1);
    sprites[12] = Sprite("alien_3", "images/alien.png", 64, 64, 468, 80, 1);
    sprites[13] = Sprite("alien_4", "images/alien.png", 64, 64, 668, 80, 1);
    sprites[14] = Sprite("end_of_array", "", 0, 0, 0, 0, 0);
    return 0;
}


//todo find a way to get rid of this
int aniarr[][2] = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {END_OF_ANI, 0}};

/*
 * initialize_sprites initializes all used animations in the animations array.
 * This sets the starting configuration for all animations used by sprites.
 *
 * returns 0 on success
 */
int initialize_animations(){
    animations[0] = Animation(aniarr, 0);
    return 0;
}

/*
 * load_sprite_textures loads all SDL_Texture members for sprites array based
 * the path listed by the img_path member
 *
 * returns 0 on success, -1 on error
 */
int load_sprite_textures() {
    int i = 0;
    while(sprites[i].get_sprite_key().compare("end_of_array") != 0) {
        if (sprites[i].load_texture() == -1) {
            return -1;
        }
        i++;
    }
    return 0;
}


/*
 * get_sprite_index iterates through the sprite array looking for a match
 * between the passed string and the key of the sprite. If a match is found,
 * the matching index is returned.
 *
 * returns the index of the passed string sprite_key, or -1 if no match.
 */
int get_sprite_index(string sprite_key) {
    int i = 0;
    while(sprites[i].get_sprite_key().compare("end_of_array") != 0) {
        if (sprites[i].get_sprite_key().compare(sprite_key) == 0) {
            return i;
        }
        i++;
    }

    if (sprite_key.compare("end_of_array") == 0) {
        return i;
    }
    return -1;
}


/*
 * initialize_board does all preloading of SDL and assets needed before
 * entering our main loop
 *
 * returns 0 if successful, -1 on error
 */
int initialize_board() {
    initialize_animations();
    initialize_sprites();
    //initialize SDL and the main window
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("Invaders: SDL_Init %s\n", SDL_GetError());
        return -1;
    } else {
        if ((g_mainwin = SDL_CreateWindow( "Invaders: SDL_CreateWindow", SDL_WINDOWPOS_UNDEFINED,
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

    if (load_sprite_textures() == -1) {
        return -1;
    }

    return 0;
}


/*
 * cleanup destroys and cleans up any resources that don't automatically
 * de-allocate memory
 */
int cleanup() {
    //todo you probably need to kill all the objects here.
    SDL_DestroyRenderer( g_renderer );
    SDL_DestroyWindow( g_mainwin );
    IMG_Quit();
    SDL_Quit();
    return 0;
}



/*
 * render_all!!!
 */
int render_all() {
    int i = 0;
    SDL_RenderClear(g_renderer);
    SDL_Rect dest_rect;
    SDL_Rect src_rect;
    while ( sprites[i].get_sprite_key().compare("end_of_array") ) {
        if (sprites[i].is_enabled() == 1) {
            dest_rect.x = sprites[i].get_sprite_x();
            dest_rect.y = sprites[i].get_sprite_y();
            dest_rect.w = sprites[i].get_sprite_width();
            dest_rect.h = sprites[i].get_sprite_height();

            src_rect = sprites[i].get_src_rect();

            SDL_RenderCopy( g_renderer, sprites[i].get_sprite_texture(), &src_rect, &dest_rect);
        }
        i++;
    }
    SDL_RenderPresent( g_renderer );
    return 0;
}




/*
 * move_all_sprites adjusts xpos and y pos for a sprite by the integers
 * indicated in xvel and yvel
 *
 */
int move_all_sprites() {
    int i = 0;
    while(sprites[i].get_sprite_key().compare("end_of_array")) {
        sprites[i].move_sprite(sprites[i].get_sprite_xvel(), sprites[i].get_sprite_yvel());
        i++;
    }
    return 0;
}


/*
 * get_board_renderer is a getter function to allow other files access to the
 * renderer.
 */
SDL_Renderer *get_board_renderer() {
    return g_renderer;
}



/*
 * update_sprites updates the position, velocity, and render value of the
 * all_sprites array in board.c according to input values and game logic.
 *
 * returns 0 on success, -1 on error
 */
int update_sprites(){
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
        //check for "x'ing out" of the window
        if(event.type == SDL_QUIT) {
            set_quit_game();
        }
        //check for key presses
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
            case SDLK_LEFT:
                sprites[get_sprite_index("ship")].set_sprite_xvel(-SHIP_SPEED);
                break;
            case SDLK_RIGHT:
                sprites[get_sprite_index("ship")].set_sprite_xvel(SHIP_SPEED);
                break;
            case SDLK_UP:
                ship_shoot();
                break;
            }
        }

        if (event.type == SDL_KEYUP) {
            switch(event.key.keysym.sym) {
            case SDLK_LEFT:
                if (sprites[get_sprite_index("ship")].get_sprite_xvel() == -SHIP_SPEED) {
                    sprites[get_sprite_index("ship")].set_sprite_xvel(0);
                }
                break;
            case SDLK_RIGHT:
                if (sprites[get_sprite_index("ship")].get_sprite_xvel() == SHIP_SPEED) {
                    sprites[get_sprite_index("ship")].set_sprite_xvel(0);
                }
                break;
            }
        }
    }
    move_all_sprites();
    check_all_collisions();
    return 0;
}


/*
 * check_collision checks the two passed sprites for x or y collisions.
 *
 * returns 0 if no collision is detected, 1 if there's a collision
 */
int check_collision(int sprite_1, int sprite_2) {
    //get edge coordinates for both sprites
    int s1left = sprites[sprite_1].get_sprite_x();
    int s2left = sprites[sprite_2].get_sprite_x();
    int s1top = sprites[sprite_1].get_sprite_y();
    int s2top = sprites[sprite_2].get_sprite_y();
    int s1right = sprites[sprite_1].get_sprite_x() + sprites[sprite_1].get_sprite_width();
    int s2right = sprites[sprite_2].get_sprite_x() + sprites[sprite_2].get_sprite_width();
    int s1bottom = sprites[sprite_1].get_sprite_y() + sprites[sprite_1].get_sprite_height();
    int s2bottom = sprites[sprite_2].get_sprite_y() + sprites[sprite_2].get_sprite_height();

    //check edges. Any edges are too far to meet, there's no collision
    if (s1right < s2left) {
        return 0;
    } else if (s1left > s2right) {
        return 0;
    } else if (s1bottom < s2top) {
        return 0;
    } else if (s1top > s2bottom) {
        return 0;
    }

    return 1;
}



/*
 * check_all_collisions checks the ship shots against the alien sprites to see
 * if any collide. If they do, aliens are killed (disabled).
 */
int check_all_collisions() {
    //nested loop to search for enabled alien sprites, then enabled shot sprites
    //if it finds both, it will check for a collision between the two.
    for (int i = 0; i < get_sprite_index("end_of_array"); i++) {
        if ((sprites[i].get_sprite_key().find("alien") != string::npos) &&
                sprites[i].is_enabled()) {
            for (int j = 0; j < get_sprite_index("end_of_array"); j++) {
                if ((sprites[j].get_sprite_key().find("shot") != string::npos) &&
                                sprites[j].is_enabled()) {
                    if (check_collision(i, j)) {
                        sprites[i].disable_sprite();
                        sprites[j].disable_sprite();
                    }
                }
            }
        }
    }

    //check if any of the shots have gone off the screen. If they have, we can
    //use them again by disabling the sprite.
    for (int i = 0; i < get_sprite_index("end_of_array"); i++) {
        if ((sprites[i].get_sprite_y() == -8) && (sprites[i].is_enabled()) &&
                (sprites[i].get_sprite_key().find("shot") != string::npos)) {
            sprites[i].disable_sprite();
        }
    }
    return 0;
}



/*
 * ship_shoot is a function that handles the ship's firing command. Enables the
 * sprite in the all_sprites function in board.cpp, and places it at the
 * correct location.
 *
 * returns 0 on success, -1 if no shot was available
 */
int ship_shoot(){
    int shot_index = -1;
    //find shot that is not currently on screen
    for (int i = 0; i < get_sprite_index("end_of_array"); i++) {
        if (!sprites[i].is_enabled() &&
                (sprites[i].get_sprite_key().find("shot") != string::npos)) {
            shot_index = i;
        }
    }
    if (shot_index == -1) {
        return -1;
    }


    //get x position for shot (x position of ship, plus 1/2 width (midship))
    int shot_x =  (sprites[get_sprite_index("ship")].get_sprite_x() +
            (sprites[get_sprite_index("ship")].get_sprite_width() / 2)) - 2;

    //get y the position that the shot should start at (y position of ship plus
    //shot image height)
    int shot_y = WIN_HEIGHT - ((WIN_HEIGHT - sprites[get_sprite_index("ship")].get_sprite_y())) - 8;

    sprites[shot_index].set_sprite_pos(shot_x, shot_y);
    sprites[shot_index].set_sprite_yvel(-20);
    sprites[shot_index].enable_sprite();
    return 0;
}

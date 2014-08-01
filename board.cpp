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
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "sprite.h"
#include "engine.h"
#include "animation.h"
#include "parsejson.h"

using namespace std;

char sprite_file[] = SPRITE_FILE;
char ani_file[] = ANI_FILE;


//globals
SDL_Window *g_mainwin = NULL;
SDL_Surface *g_window_surface = NULL;
SDL_Renderer *g_renderer = NULL;

//function declarations
int ship_shoot();
int check_all_collisions();
int aliens_shoot();
int check_win_lose();

//data structure declarations
static vector<Sprite> sprites;
static vector<Animation> animations;

/*
 * initialize_sprites initializes all used sprites in the sprites vector. This
 * sets the starting configuration for all images on the screen.
 *
 * It's worth noting that when rendering, the sprites are layered on top of
 * one another, starting with index 0, then iterating through the final
 * active sprite. This means a sprite with lower index will be overlapped by a
 * sprite with higher index.
 *
 * returns 0 on success
 */
int initialize_sprites() {
	for (int i = 0; i < get_json_array_len(sprite_file); i++) {
		sprites.push_back(sprite_from_json(sprite_file, i, animations));
	}
	return 0;
}



/*
 * initialize_sprites initializes all used animations in the animations vector.
 * This sets the starting configuration for all animations used by sprites.
 *
 * returns 0 on success
 */
int initialize_animations(){
	//todo get rid of this
    //animations.push_back(Animation(aniarr, 0));
	for (int i = 0; i < get_json_array_len(ani_file); i++) {
		animations.push_back(animation_from_json(ani_file, i));
	}
    return 0;
}

/*
 * load_sprite_textures loads all SDL_Texture members for sprites array based
 * the path listed by the img_path member
 *
 * returns 0 on success, -1 on error
 */
int load_sprite_textures() {
    for(u_int i = 0; i < sprites.size(); i++) {
        if (sprites[i].load_texture() == -1) {
            return -1;
        }
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
    for(u_int i = 0; i < sprites.size(); i++) {
        if (sprites[i].get_sprite_key().compare(sprite_key) == 0) {
            return i;
        }
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

    srand(time(NULL));

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
    SDL_RenderClear(g_renderer);
    SDL_Rect dest_rect;
    SDL_Rect src_rect;
    for(u_int i = 0; i < sprites.size(); i++) {
        if (sprites[i].is_enabled() == 1) {
            dest_rect.x = sprites[i].get_sprite_x();
            dest_rect.y = sprites[i].get_sprite_y();
            dest_rect.w = sprites[i].get_sprite_width();
            dest_rect.h = sprites[i].get_sprite_height();

            src_rect = sprites[i].get_src_rect();

            SDL_RenderCopy( g_renderer, sprites[i].get_sprite_texture(), &src_rect, &dest_rect);
        }
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
    for(u_int i = 0; i < sprites.size(); i++) {
        sprites[i].move_sprite(sprites[i].get_sprite_xvel(), sprites[i].get_sprite_yvel());
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
    aliens_shoot();
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
	for(u_int i = 0; i < sprites.size(); i++) {
        if ((sprites[i].get_sprite_key().find("alien") != string::npos) &&
                sprites[i].is_enabled()) {
        	for(u_int j = 0; j < sprites.size(); j++) {
                if ((sprites[j].get_sprite_key().find("ship_shot") != string::npos) &&
                                sprites[j].is_enabled()) {
                    if (check_collision(i, j)) {
                        sprites[i].disable_sprite();
                        sprites[j].disable_sprite();
                    }
                }
            }
        }
    }

	//loop to check for enabled alien shots, and to see if they have a collision with the ship
	int ship_index = get_sprite_index("ship");
	for(u_int i = 0; i < sprites.size(); i++) {
	    if ((sprites[i].get_sprite_key().find("al_shot") != string::npos) && sprites[i].is_enabled()) {
	        if (check_collision(ship_index, i)) {
	            sprites[i].disable_sprite();
	            sprites[ship_index].disable_sprite();
	        }
	    }
	}

    //check if any of the shots have gone off the screen. If they have, we can
    //use them again by disabling the sprite.
	for(u_int i = 0; i < sprites.size(); i++) {
        if ((sprites[i].get_sprite_y() == -8) && (sprites[i].is_enabled()) &&
                (sprites[i].get_sprite_key().find("ship_shot") != string::npos)) {
            sprites[i].disable_sprite();
        }
        if ((sprites[i].get_sprite_y() == (WIN_HEIGHT)) && (sprites[i].is_enabled()) &&
                (sprites[i].get_sprite_key().find("al_shot") != string::npos)) {
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
    //don't allow shooting from a ghost-ship. It's creepy.
    if (!sprites[get_sprite_index("ship")].is_enabled()) {
        return -1;
    }

    int shot_index = -1;
    //find shot that is not currently on screen
    for(u_int i = 0; i < sprites.size(); i++) {
        if (!sprites[i].is_enabled() &&
                (sprites[i].get_sprite_key().find("ship_shot") != string::npos)) {
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


/*
 * get_free_alien_shot iterates through the sprites and returns the first
 * alien_shot sprite that is not currently enabled.
 */
int get_free_alien_shot() {
    for (u_int i = 0; i < sprites.size(); i++) {
        if ((sprites[i].get_sprite_key().find("al_shot") != string::npos) && (!sprites[i].is_enabled())) {
            return i;
        }
    }
    return -1;
}


/*
 * aliens_shoot has a 1/50 chance of causing a shot to be fired by each alien.
 * If each shot is already active, nothing will be done.
 */
int aliens_shoot(){
    for (u_int i = 0; i < sprites.size(); i++) {
        if ((sprites[i].get_sprite_key().find("alien") != string::npos) && sprites[i].is_enabled()) {
            if ((rand() % 50) == 1) {
                int ashot_index;
                if ((ashot_index = get_free_alien_shot()) == -1) {
                    //if we make it here, all alien shots are already out there
                    return -1;
                }
                int new_shot_x = (sprites[i].get_sprite_x() + (sprites[i].get_sprite_width() / 2));
                sprites[ashot_index].set_sprite_x(new_shot_x);
                sprites[ashot_index].set_sprite_y(sprites[i].get_sprite_y() + (sprites[i].get_sprite_height() / 2));
                sprites[ashot_index].set_sprite_yvel(15);
                sprites[ashot_index].enable_sprite();
            }
        }
    }
    return 0;
}


/*
 * check_win_lose checks for win conditions (all alien sprites are disabled),
 * and lose conditions (ship sprite is disabled). If either is encountered, the
 * appropriate win/lose image is displayed.
 */
int check_win_lose() {
    if (!sprites[get_sprite_index("ship")].is_enabled()) {
        int lose_index = get_sprite_index("lose");
        sprites[lose_index].set_sprite_x((WIN_WIDTH / 2) - (sprites[lose_index].get_sprite_width() / 2));
        sprites[lose_index].set_sprite_y((WIN_HEIGHT / 2) - (sprites[lose_index].get_sprite_height() / 2));
        sprites[lose_index].enable_sprite();
        return LOSE;
    } else {
        for (u_int i = 0; i < sprites.size(); i++) {
            if ((sprites[i].get_sprite_key().find("alien") != string::npos) && sprites[i].is_enabled()) {
                return 0;
            }
        }

        //if we've gotten this far, none of our aliens are enabled.
        int win_index = get_sprite_index("win");
        sprites[win_index].set_sprite_x((WIN_WIDTH / 2) - (sprites[win_index].get_sprite_width() / 2));
        sprites[win_index].set_sprite_y((WIN_HEIGHT / 2) - (sprites[win_index].get_sprite_height() / 2));
        sprites[win_index].enable_sprite();
        return WIN;
    }
    return 0;
}

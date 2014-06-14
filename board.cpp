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
#include "sprite.h"
#include "engine.h"

using namespace std;


//globals
SDL_Window *g_mainwin = NULL;
SDL_Surface *g_window_surface = NULL;
SDL_Renderer *g_renderer = NULL;

//function declarations
int ship_shoot();
int check_all_collisions();


static Sprite sprites[] = {
        Sprite("background", "images/space.png", 640, 640, 0, 0, 1),
        Sprite("ship", "images/ship.png", 48, 48, ((WIN_WIDTH / 2) - 24), (WIN_HEIGHT - 48), 1),
        Sprite("shot_1", "images/shot.png", 4, 8, 320, 460, 0),
        Sprite("shot_2", "images/shot.png", 4, 8, 320, 460, 0),
        Sprite("shot_3", "images/shot.png", 4, 8, 320, 460, 0),
        Sprite("alien_1", "images/alien.png", 64, 64, 160, 80, 1),
        Sprite("alien_2", "images/alien.png", 64, 64, 480, 80, 1),
        Sprite("end_of_array", "", 0, 0, 0, 0, 0)
};


/*
 * load_sprite_textures loads all SDL_Texture members for sprites array based
 * the path listed by the img_path member
 *
 * returns 0 on success, -1 on error
 */
int load_sprite_textures() {
    int i = 0;
    printf("about to load textures.\n");
    while(sprites[i].get_sprite_key().compare("end_of_array") != 0) {
        printf("calling load_texture() for %s.\n", sprites[i].get_img_path().c_str());
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
    return -1;
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
    while ( sprites[i].get_sprite_key().compare("end_of_array") ) {
        if (sprites[i].is_enabled() == 1) {
            dest_rect.x = sprites[i].get_sprite_x();
            dest_rect.y = sprites[i].get_sprite_y();
            dest_rect.w = sprites[i].get_sprite_width();
            dest_rect.h = sprites[i].get_sprite_height();
            SDL_RenderCopy( g_renderer, sprites[i].get_sprite_texture(), NULL, &dest_rect);
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
    if (sprites[get_sprite_index("shot")].is_enabled() ||
            sprites[get_sprite_index("shot_2")].is_enabled() ||
            sprites[get_sprite_index("shot_3")].is_enabled()) {
        if (sprites[get_sprite_index("alien_1")].is_enabled()) {
            if (check_collision(get_sprite_index("shot_1"), get_sprite_index("alien_1")) ||
                    check_collision(get_sprite_index("shot_2"), get_sprite_index("alien_1")) ||
                    check_collision(get_sprite_index("shot_3"), get_sprite_index("alient_1"))) {
                //todo this could disable the wrong shot....
                sprites[get_sprite_index("shot")].disable_sprite();
                sprites[get_sprite_index("alien_1")].disable_sprite();
            }
        }
        if(sprites[get_sprite_index("alien_2")].is_enabled()) {
            if (check_collision(get_sprite_index("shot"), get_sprite_index("alien_2")) ||
                    check_collision(get_sprite_index("shot_2"), get_sprite_index("alien_2")) ||
                    check_collision(get_sprite_index("shot_3"), get_sprite_index("alien_2"))) {
                sprites[get_sprite_index("shot")].disable_sprite();
                sprites[get_sprite_index("alien_2")].disable_sprite();
            }
        }
    }

    //check if any of the shots have gone off the screen. If they have, we can
    //use them again by disabling the sprite.
    for (int i = get_sprite_index("shot"); i <= get_sprite_index("shot_3"); i++) {
        if ((sprites[i].get_sprite_y() == -8) && (sprites[i].is_enabled())) {
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
    for (int i = get_sprite_index("shot"); i <= get_sprite_index("shot_3"); i++) {
        if (!sprites[i].is_enabled()) {
            shot_index = i;
        }
    }
    printf("got shot index of %i\n", shot_index);
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





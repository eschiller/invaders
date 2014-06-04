/*
 * engine.cpp
 *
 *  Created on: May 31, 2014
 *      Author: eschiller
 */
#include <SDL2/SDL.h>

#include "board.h"
#include "timer.h"

int quit_game = 0;

int ship_shoot();
int check_collision(int sprite_1, int sprite_2);
int check_all_collisions();


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
            quit_game = 1;
        }
        //check for key presses
        if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
            case SDLK_LEFT:
                move_sprite(SHIP, -10, 0);
                break;
            case SDLK_RIGHT:
                move_sprite(SHIP, 10, 0);
                break;
            case SDLK_UP:
                ship_shoot();
                break;
            }
        }
    }
    move_all_sprites();
    check_all_collisions();
    return 0;
}


/*
 * game_loop is the main game loop! It first calls update_sprites to make any
 * adjustments according to logic, then it waits until the next frame should be
 * displayed, then it renders.
 *
 * returns 0 on success
 */
int game_loop(){
    set_time();
    while ( !quit_game ) {
        update_sprites();
        if (time_elapsed() < MS_PER_FRAME ) {
            wait_for_frame();
        }
        render_all();
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
    for (int i = SHOT; i <= SHOT3; i++) {
        if (!is_enabled(i)) {
            shot_index = i;
        }
    }
    printf("got shot index of %i\n", shot_index);
    if (shot_index == -1) {
        return -1;
    }


    //get x position for shot (x position of ship, plus 1/2 width (midship))
    int shot_x =  (get_sprite_x(SHIP) + (get_sprite_width(SHIP) / 2)) - 2;

    //get y the position that the shot should start at (y position of ship plus
    //shot image height)
    int shot_y = WIN_HEIGHT - ((WIN_HEIGHT - get_sprite_y(SHIP))) - 8;

    set_sprite_pos(shot_index, shot_x, shot_y);
    enable_sprite(shot_index);
    return 0;
}


/*
 * check_collision checks the two passed sprites for x or y collisions.
 *
 * returns 0 if no collision is detected, 1 if there's a collision
 */
int check_collision(int sprite_1, int sprite_2) {
    //get edge coordinates for both sprites
    int s1left = get_sprite_x(sprite_1);
    int s2left = get_sprite_x(sprite_2);
    int s1top = get_sprite_y(sprite_1);
    int s2top = get_sprite_y(sprite_2);
    int s1right = get_sprite_x(sprite_1) + get_sprite_width(sprite_1);
    int s2right = get_sprite_x(sprite_2) + get_sprite_width(sprite_2);
    int s1bottom = get_sprite_y(sprite_1) + get_sprite_height(sprite_1);
    int s2bottom = get_sprite_y(sprite_2) + get_sprite_height(sprite_2);

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
    if (is_enabled(SHOT) || is_enabled(SHOT2) || is_enabled(SHOT3)) {
        if (is_enabled(ALIEN_1)) {
            if (check_collision(SHOT, ALIEN_1) ||
                    check_collision(SHOT2, ALIEN_1) ||
                    check_collision(SHOT3, ALIEN_1)) {
                disable_sprite(SHOT);
                disable_sprite(ALIEN_1);
            }
        }
        if(is_enabled(ALIEN_2)) {
            if (check_collision(SHOT, ALIEN_2) ||
                    check_collision(SHOT2, ALIEN_2) ||
                    check_collision(SHOT3, ALIEN_2)) {
                disable_sprite(SHOT);
                disable_sprite(ALIEN_2);
            }
        }
    }

    //check if any of the shots have gone off the screen. If they have, we can
    //use them again by disabling the sprite.
    for (int i = SHOT; i <= SHOT3; i++) {
        if ((get_sprite_y(i) == -8) && (is_enabled(i))) {
            disable_sprite(i);
        }
    }
    return 0;
}

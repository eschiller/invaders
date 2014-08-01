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
        set_time();
        check_win_lose();
    }
    return 0;
}


/*
 * set_quit_game sets the quit_game variable to 1, causing the game to exit
 */
void set_quit_game(){
    quit_game = 1;
}





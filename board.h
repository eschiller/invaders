/*
 * board.h
 *
 *  Created on: May 28, 2014
 *      Author: eschiller
 */

#ifndef BOARD_H_
#define BOARD_H_

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define WIN 1
#define LOSE 2

#define SHIP_SPEED 14
#define SPRITE_FILE "jsondata/sprites.json"
#define ANI_FILE "jsondata/animations.json"

int initialize_board();
int render_all();
SDL_Renderer *get_board_renderer();
int update_sprites();
int check_win_lose();


#endif /* BOARD_H_ */

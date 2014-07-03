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

#define SHIP_SPEED 14

int initialize_board();
int render_all();
SDL_Renderer *get_board_renderer();
int update_sprites();

#endif /* BOARD_H_ */

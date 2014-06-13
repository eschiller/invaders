/*
 * board.h
 *
 *  Created on: May 28, 2014
 *      Author: eschiller
 */

#ifndef BOARD_H_
#define BOARD_H_

#define END_OF_ARRAY -1
#define BACKGROUND 0
#define SHIP 1
#define SHOT 2
#define SHOT2 3
#define SHOT3 4
#define ALIEN_1 5
#define ALIEN_2 6

#define WIN_WIDTH 640
#define WIN_HEIGHT 640

int initialize_board();
int render_all();
SDL_Renderer *get_board_renderer();
int update_sprites();

#endif /* BOARD_H_ */

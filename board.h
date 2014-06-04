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
int move_sprite(int sprite_index, int x_delta, int y_delta);
int move_all_sprites();
void enable_sprite(int sprite_index);
void disable_sprite(int sprite_index);
int get_sprite_x(int sprite_index);
int get_sprite_y(int sprite_index);
int get_sprite_width(int sprite_index);
int get_sprite_height(int sprite_index);
int set_sprite_x(int sprite_index, int newx);
int set_sprite_y(int sprite_index, int newy);
int set_sprite_pos(int sprite_index, int newx, int newy);
int is_enabled(int sprite_index);

#endif /* BOARD_H_ */

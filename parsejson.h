/*
 * parsejson.h
 *
 *  Created on: Jul 5, 2014
 *      Author: eschiller
 */

#ifndef PARSEJSON_H_
#define PARSEJSON_H_

#include "sprite.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Sprite sprite_from_json(char *filepath, int index, const vector<Animation> &animations);
Animation animation_from_json(char *filepath, int index);
int get_json_array_len(char *filepath);


#endif /* PARSEJSON_H_ */

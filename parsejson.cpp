/*
 * parsejson.cpp
 *
 *  Created on: Jul 5, 2014
 *      Author: eschiller
 */
#include <jansson.h>
#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "sprite.h"
#include "animation.h"

using namespace std;


Sprite sprite_from_json(char *filepath, int index, const vector<Animation> &animations) {
	json_t *root, *sprite_object, *key_object,*img_path_object, *width_object,
		*height_object, *xpos_object, *ypos_object, *render_object, *animation_object;
	json_error_t jerror;
	string sprite_key, sprite_img_path;
	int sprite_width = 0;
	int sprite_height = 0;
	int sprite_xpos = 0;
	int sprite_ypos = 0;
	int sprite_render = 0;
	int animation_index = -1;

	if ((root = json_load_file(filepath, 0, &jerror)) == NULL) {
		fprintf(stderr, "invaders: json_load_file: %s\n", jerror.text);
	}

	if (!(json_is_array(root))) {
		fprintf(stderr, "invaders: error, root is not array.\n");
		json_decref(root);
		return Sprite();
	} else {
		sprite_object = json_array_get(root, index);
		if (!(json_is_object(sprite_object))) {
			fprintf(stderr, "invaders: json_is_object: couldn't get object from json array.\n");
			json_decref(root);
			return Sprite();
		} else {
			//if we've made it this far everything's looking good. Start
			//iterating through all sprite creation info, assign to variables,
			//and create a new sprite based on assigned values.

			key_object = json_object_get(sprite_object, "key");
			sprite_key = json_string_value(key_object);
			img_path_object = json_object_get(sprite_object, "img_path");
			sprite_img_path = json_string_value(img_path_object);

			width_object = json_object_get(sprite_object, "width");
			sprite_width = json_integer_value(width_object);

			height_object = json_object_get(sprite_object, "height");
			sprite_height = json_integer_value(height_object);

			xpos_object = json_object_get(sprite_object, "xpos");
			sprite_xpos = json_integer_value(xpos_object);

			ypos_object = json_object_get(sprite_object, "ypos");
			sprite_ypos = json_integer_value(ypos_object);

			render_object = json_object_get(sprite_object, "render");
			sprite_render = json_integer_value(render_object);

			animation_object = json_object_get(sprite_object, "animation");
			if (json_is_number(animation_object)) {
				animation_index = json_integer_value(animation_object);
			}

			//if there's no animation, use the non-ani constructor, otherwise
			//call the constructor passing the animation
			if (animation_index == -1) {
				json_decref(root);
				return Sprite(sprite_key, sprite_img_path, sprite_width, sprite_height,
						sprite_xpos, sprite_ypos, sprite_render);
			} else {
				json_decref(root);
				return Sprite(sprite_key, sprite_img_path, sprite_width, sprite_height,
						sprite_xpos, sprite_ypos, sprite_render, animations[animation_index]);
			}
		}
	}
	json_decref(root);
	return Sprite();
}


Animation animation_from_json(char *filepath, int index) {
	int script[MAX_SCRIPT_LEN][2];
	int loop_frame;
	json_t *root, *animation_object, *loop_frame_object, *ani_script_object,
			*script_frame_object, *count_object, *clip_object;
	json_error_t jerror;

	if ((root = json_load_file(filepath, 0, &jerror)) == NULL) {
			fprintf(stderr, "invaders: json_load_file: %s\n", jerror.text);
		}

	if (!(json_is_array(root))) {
		fprintf(stderr, "invaders: error, root is not array.\n");
		return Animation();
	} else {
		animation_object = json_array_get(root, index);
		if (!(json_is_object(animation_object))) {
			fprintf(stderr, "invaders: json_is_object: couldn't get object from json array.\n");
			json_decref(root);
			return Animation();
		} else {
			//animation_script get's parsed here.
			ani_script_object = json_object_get(animation_object, "ani_script");
			if (!(json_is_array(ani_script_object))) {
				fprintf(stderr, "invaders: error, aniscript is not an array.\n");
				json_decref(root);
				return Animation();
			}
			int count, clip;
			//loop through all json objects that make individual frames
			for (u_int i = 0; i < json_array_size(ani_script_object); i++) {
				script_frame_object = json_array_get(ani_script_object, i);
				count_object = json_object_get(script_frame_object, "duration");
				clip_object = json_object_get(script_frame_object, "clip");
				count = json_integer_value(count_object);
				clip = json_integer_value(clip_object);
				script[i][DURATION] = count;
				script[i][CLIP] = clip;
			}
			//loop_frame parsing
			loop_frame_object = json_object_get(animation_object, "loop_frame");
			loop_frame = json_integer_value(loop_frame_object);
			//This is the return we want.
			json_decref(root);
			return Animation(script, loop_frame);
		}
	}
	json_decref(root);
	return Animation();

}



int get_json_array_len(char *filepath) {
	json_t *root;
	json_error_t jerror;
	int size;

	if ((root = json_load_file(filepath, 0, &jerror)) == NULL) {
		fprintf(stderr, "invaders: json_load_file: %s\n", jerror.text);
		return -1;
	}

	if (!(json_is_array(root))) {
		fprintf(stderr, "invaders: json doesn't contain array.\n");
		return -1;
	}

	size =  json_array_size(root);
	json_decref(root);
	return size;
}


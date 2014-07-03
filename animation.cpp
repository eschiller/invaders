/*
 * animation.cpp
 *
 *  Created on: Jun 27, 2014
 *      Author: eschiller
 */
#include <stdio.h>

#include "animation.h"

/*
 * default Animation constructor. This should generally be avoided in favor the
 * Animation(int ani_script[][2], int loop_frame) constructor.
 */
Animation::Animation() {
    this->curr_frame = 0;
    this->curr_frame_count = 0;
    this->loop_frame = NO_LOOP;
    this->ani_script[0][CLIP] = 0;
    this->ani_script[0][DURATION] = END_OF_ANI;
}


/*
 * This is the primary constructor that should be used to initialize new
 * Animation objects. ani_script should contain a 2 dimensional array,
 * terminating with END_OF_ANI in ani_script[final_index][DURATION]
 *
 * loop_frame indicates the index of the animation frame that should be
 * returned when reaching the terminal frame. If the animation does not loop,
 * the value NO_LOOP should be passed.
 */
Animation::Animation(int ani_script[][2], int loop_frame) {
    int aFrame = 0;
    while(ani_script[aFrame][DURATION] != END_OF_ANI) {
        this->ani_script[aFrame][DURATION] = ani_script[aFrame][DURATION];
        this->ani_script[aFrame][CLIP] = ani_script[aFrame][CLIP];
        aFrame++;
    }
    curr_frame = 0;
    curr_frame_count = 0;
    this->ani_script[aFrame][CLIP] = ani_script[aFrame][CLIP];
    this->ani_script[aFrame][DURATION] = END_OF_ANI;

    this->loop_frame = loop_frame;
}


/*
 * print_ani prints the entire script of the animation. This is only used for
 * debugging.
 */
void Animation::print_ani() {
    int aFrame = 0;
    while(aFrame < 20) {
        printf("%i, %i\n", ani_script[aFrame][CLIP], ani_script[aFrame][DURATION]);
        aFrame++;
    }
}


/*
 * update_ani works through a single frame of the animation, adjusting the
 * frame count, current image clip (index of the sprite sheet), and looping
 * as inicated by the animation.
 *
 * returns clip (sprite_sheet index) of the current animation frame
 */
int Animation::update_ani() {
    //todo something about this causes two 0 returns in a row. Fix algorithm.

    curr_frame_count++;
    //if we're at the end of the animation, either loop, or return clip
    if (ani_script[curr_frame][DURATION] == END_OF_ANI) {
        if (loop_frame == NO_LOOP) {
            return ani_script[curr_frame][CLIP];
            //make sure we don't exceed bounds of int type
            curr_frame_count--;
        } else {
            curr_frame = loop_frame;
            curr_frame_count = 0;
        }
    //if we've reached the end of the frame duration for the current clip,
    //increment the clip
    } else if (ani_script[curr_frame][DURATION] == curr_frame_count) {
        curr_frame++;
        curr_frame_count = 0;
    }
    return ani_script[curr_frame][CLIP];
}


/*
 * is_empty is used to tell if a sprite has no set animation it should use.
 *
 * returns true if there's no animation, false if an animation is present.
 */
bool Animation::is_empty() {
    if (this->ani_script[0][DURATION] == END_OF_ANI) {
        return true;
    } else {
        return false;
    }
}


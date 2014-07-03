/*
 * animation.h
 *
 *  Created on: Jun 27, 2014
 *      Author: eschiller
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#define DURATION 0
#define CLIP 1
#define MAX_SCRIPT_LEN 128
#define END_OF_ANI -1
#define NO_LOOP -1


class Animation {
private:
    int ani_script[MAX_SCRIPT_LEN][2];
    int curr_frame;
    int curr_frame_count;
    int loop_frame;

public:
    Animation();
    Animation(int ani_script[][2], int loop_frame);
    int update_ani();
    bool is_empty();
    void print_ani();
};


#endif /* ANIMATION_H_ */

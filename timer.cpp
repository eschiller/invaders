/*
 * timer.cpp
 *
 *  Created on: May 28, 2014
 *      Author: eschiller
 */
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include "timer.h"


//g_start_time holds the start time of the timer in ms
double g_start_time;

//prototypes not in header
double get_time_in_ms();


/*
 * set_time sets the initial time in milliseconds
 */
int set_time(){
    g_start_time = get_time_in_ms();
    return 0;
}


/*
 * get_time_in_ms uses the gettimeofday system call to get the current epoch
 * time in milliseconds
 */
double get_time_in_ms() {
    double time_in_ms = 0;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        perror("invaders");
        return -1;
    }

    //tv returns seconds and microseconds, which we need to convert
    time_in_ms += tv.tv_sec * 1000;
    time_in_ms += (tv.tv_usec / 1000);
    return time_in_ms;
}


/*
 * time_elapsed returns the difference between the start time and the current
 */
int time_elapsed() {
    return get_time_in_ms() - g_start_time;
}


/*
 * checks to see if we're ready to render another frame. If we're not, it waits
 * until we are.
 *
 * returns either the number of ms that it waited, or -1 if error.
 */
int wait_for_frame() {
    int time_to_wait;
    if (time_elapsed() < MS_PER_FRAME) {
        time_to_wait = MS_PER_FRAME - time_elapsed();
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = time_to_wait * 1000000;
        if (nanosleep(&ts, NULL) != 0) {
            perror("Invaders");
            return -1;
        }

        //timer must be re-set so we don't automatically frame every time
        set_time();
        return time_to_wait;
    }

    set_time();
    return 0;
}




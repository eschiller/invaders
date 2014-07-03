/*
 * timer.h
 *
 *  Created on: May 28, 2014
 *      Author: eschiller
 */

#ifndef TIMER_H_
#define TIMER_H_

/*
 * MPT short of milliseconds-per-tick. A "tick" is and abstract unit of time,
 * which will be used to measure events. For example, we might render a new
 * frame every four ticks, and move our sprites every eight ticks
 */
#define MS_PER_FRAME 50

int set_time();
int time_elapsed();
int wait_for_frame();



#endif /* TIMER_H_ */

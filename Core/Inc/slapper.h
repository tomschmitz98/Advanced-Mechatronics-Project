/*
 * slapper.h
 *
 *  Created on: Dec 4, 2023
 *      Author: Tom
 */

#ifndef SLAPPER_H_
#define SLAPPER_H_

#include <stdbool.h>

typedef enum {
    NO_ACTION,
    SENSORS_USER_QUERY,
    PRINT_PAUSED,
    PRINT_RED,
    START_REACTION,
    START_ACTUATOR,
    RESET_ACTUATOR,
    UPDATE_SCORE_CPU,
    UPDATE_SCORE_USER,
    QUERY_PLAY_AGAIN
} slapper_action_t;

slapper_action_t run_slapper(bool start, bool pause, bool actuator_done);

#endif /* SLAPPER_H_ */

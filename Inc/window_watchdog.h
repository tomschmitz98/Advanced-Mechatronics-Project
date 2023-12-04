/*
 * window_watchdog.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef WINDOW_WATCHDOG_H_
#define WINDOW_WATCHDOG_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

typedef enum {
	WWD_DIV_1 = 0,
	WWD_DIV_2 = 1,
	WWD_DIV_4 = 2,
	WWD_DIV_8 = 3,
} wwd_div_t;

typedef struct {
	bool early_wakeup;
	wwd_div_t div;
	uint8_t window;
	uint8_t reset_value;
} wwd_configs_t;

void config_window_dog(wwd_configs_t config);

void enable_window_dog(void);

void enable_window_dog_kicking(void);
void disable_window_dog_kicking(void);

void try_kicking_window_dog(void);

bool check_window_early_wakeup(void);
void acknowledge_early_wakeup(void);

void kick_the_window_dog(void);

#endif /* WINDOW_WATCHDOG_H_ */

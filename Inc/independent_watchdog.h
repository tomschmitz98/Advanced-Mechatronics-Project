/*
 * independent_watchdog.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef INDEPENDENT_WATCHDOG_H_
#define INDEPENDENT_WATCHDOG_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

#define IWDG_PVU BIT0
#define IWDG_RVU BIT1

typedef enum {
	IWDG_DIV4 = 0,
	IWDG_DIV8 = 1,
	IWDG_DIV16 = 2,
	IWDG_DIV32 = 3,
	IWDG_DIV64 = 4,
	IWDG_DIV128 = 5,
	IWDG_DIV256 = 6,
	IWDG_DIV256_ALIAS = 7,
} iwdg_prescale_t;

typedef struct {
	iwdg_prescale_t prescaler;
	uint16_t reload;
} iwdg_configs_t;

void config_independent_dog(void);
void disable_independent_dog_kicking(void);
void enable_independent_dog_kicking(void);
void kick_the_independent_dog(void);
uint8_t check_independent_dog_status(uint8_t mask);

#endif /* INDEPENDENT_WATCHDOG_H_ */

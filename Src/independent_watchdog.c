/*
 * independent_watchdog.c
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#include "independent_watchdog.h"
#include <stdint.h>
#include <stdbool.h>

#define WATCHDOG_BASE(x) *(((uint32_t *)0x40003000) + x)
#define IWDG_KR 0
#define IWDG_PR 1
#define IWDG_RLR 2
#define IWDG_SR 3

#define RELOAD_KEY 0xAAAA
#define MODIFY_KEY 0x5555
#define START_KEY 0xCCCC
#define MAX_TIMEOUT 0xFFF

static bool allow_dog_kicking = true;

static void write_iwdg_prescale(iwdg_prescale_t prescale)
{
	WATCHDOG_BASE(IWDG_KR) = MODIFY_KEY;

	while (check_independent_dog_status(IWDG_PVU))
		;

	WATCHDOG_BASE(IWDG_PR) = ((uint32_t)prescale) & 0x7;
}

static void write_iwdg_reload(uint16_t reload)
{
	WATCHDOG_BASE(IWDG_KR) = MODIFY_KEY;

	while (check_independent_dog_status(IWDG_RVU))
			;

	WATCHDOG_BASE(IWDG_RLR) = (uint32_t)reload;
}

void config_independent_dog(iwdg_configs_t configs)
{
	write_iwdg_prescale(configs.prescaler);
	write_iwdg_reload(configs.reload);

	// Wait for everything to enter into steady state
	while (check_independent_dog_status(IWDG_RVU | IWDG_PVU))
		;

	// Restart the dog
	WATCHDOG_BASE(IWDG_KR) = START_KEY;

	enable_independent_dog_kicking();
	kick_the_independent_dog();
}

void disable_independent_dog_kicking(void)
{
	allow_dog_kicking = false;
}

void enable_independent_dog_kicking(void)
{
	allow_dog_kicking = true;
}

void kick_the_independent_dog(void)
{
	if (allow_dog_kicking)
	{
		WATCHDOG_BASE(IWDG_KR) = RELOAD_KEY;
	}
}

uint8_t check_independent_dog_status(uint8_t mask)
{
	return (uint8_t)(WATCHDOG_BASE(IWDG_SR) & mask);
}

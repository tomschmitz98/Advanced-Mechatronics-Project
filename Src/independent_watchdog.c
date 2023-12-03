/*
 * independent_watchdog.c
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#include "independent_watchdog.h"
#include <stdint.h>

#define WATCHDOG_BASE(x) *(((uint32_t *)0x40003000) + x)
#define IWDG_KR 0
#define IWDG_PR 1
#define IWDG_RLR 2
#define IWDR_SR 3

#define RELOAD_KEY 0xAAAA
#define MAX_TIMEOUT 0xFFF

void kick_the_dog(void)
{
	WATCHDOG_BASE(IWDG_KR) = RELOAD_KEY;
}

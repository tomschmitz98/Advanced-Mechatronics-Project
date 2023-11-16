/*
 * interrupt.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include "interrupt.h"
#include "general_timers.h"
#include "core_m4.h"
#include "exti.h"

volatile uint64_t gEvents = 0;

void config_interrupts(void)
{
	disable_global_irq();
	enable_global_irq();
}

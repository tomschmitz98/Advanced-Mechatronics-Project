/*
 * timers.c
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include <stdbool.h>
#include "timers.h"
#include "general_timers.h"
#include "gpio.h"
#include "core_m4.h"
#include "productDef.h"

#define WATCHDOG_RESET 30000

const general_timer_attr_t tim2 = {
		.autoReload = true,
		.direction = UP_COUNTER,
		.prescaler = 89,
		.auto_reload_value = 200,
		.enableAfterConfig = false,
		.interruptEnableMask = UIE
};

const irq_info_t tim2_irq = {
		INT_NUM_TIM2,
		10
};

static volatile uint16_t watchdog_count = WATCHDOG_RESET;
static bool allow_dog_kicking = true;
static bool measure = false;
static volatile uint32_t measurement = 0;
static volatile uint32_t timems = 0;

void TIM2_IRQHandler(void)
{
	if (checkTimerStatus(TIMER2, UIF))
	{
		gEvents |= E_HEARTBEAT;
		watchdog_count--;
		timems++;
	}
	if (!watchdog_count)
	{
		reset_system();
	}

	// Clear erroneous status
	clearTimerStatusRegister(TIMER2);
}

void init_heartbeat(void)
{
	// Configure Heart beat
	timems = 0;
	configure_interrupt(tim2_irq);
	configureGeneralTimer(TIMER2, tim2);
	enableTimer(TIMER2);
}

void kick_the_watchdog(void)
{
	disable_global_irq();
	if (allow_dog_kicking)
	{
		watchdog_count = WATCHDOG_RESET;
	}
	enable_global_irq();
}

uint32_t read_heartbeat(void)
{
	return (uint32_t)getCounterValue(TIMER2);
}

void start_measurement(void)
{
	disable_global_irq();
	measurement = timems;
	measure = true;
	enable_global_irq();
}

void stop_measurement(void)
{
	disable_global_irq();
	measurement = timems - measurement;
	measure = false;
	enable_global_irq();
}

uint32_t read_measurement(void)
{
	return measurement;
}

uint32_t current_ts(void)
{
	return timems;
}
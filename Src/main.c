/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include <stdint.h>
#include <stdbool.h>
#include "timers.h"
#include "productDef.h"
#include "stm_utils.h"
#include "gpio.h"
#include "stm_rcc.h"
#include "window_watchdog.h"
#include "core_m4.h"
#include "reaction.h"
#include "ir_sensors.h"

#define ALLOW_PREEMPTION true

#if ALLOW_PREEMPTION
#define CONTINUE continue
#else
#define CONTINUE
#endif

volatile uint32_t gEvents = 0;


const gpio_config_t led0_configs = {
		14,
		0,
		LOW,
		bank_b,
		output,
		push_pull,
		high_speed,
		no_pull
};

void init(void)
{
	init_gpio(led0_configs);

	if (check_clock_flag(SOFTWARE_RESET))
	{
		clear_clock_flags();
		setPin(bank_b, 14);
	}
	clear_clock_flags();

	initialize_ir_sensors();
	config_reaction();

	init_heartbeat();
}

int main(void)
{
	init();

	for(uint32_t i = 0; i < 10000000; i++);
	setPin(bank_b, 14);
	start_reaction();

	while(1)
	{
		if (gEvents & E_EXTI9_5)
		{
			read_reaction();
			gEvents &= ~E_EXTI9_5;
		}
		if (gEvents & E_HEARTBEAT)
		{
			// Do something
			gEvents &= ~E_HEARTBEAT;
			CONTINUE;
		}
	}
}

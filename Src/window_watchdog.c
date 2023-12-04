/*
 * window_watchdog.c
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#include "window_watchdog.h"
#include "stm_utils.h"
#include "stm_rcc.h"

#define WWDG_BASE(x) *(((uint32_t *)0x40002C00) + x)
#define WWDG_CR 0
#define WWDG_CFR 1
#define WWDG_SR 2

#define WWDG_VALUE_MASK 0x3F
#define WWDG_RESET_VALUE 0x40
#define WWDG_WINDOW_MASK 0x7F

static uint8_t reset_value = 0x0;
static uint8_t window = 0x0;
static bool allow_dog_kicking = true;

static void set_ewi(void)
{
	WWDG_BASE(WWDG_CFR) |= BIT9;
}

static void set_timer_base(wwd_div_t base)
{
	WWDG_BASE(WWDG_CFR) &= ~(BIT8 | BIT7);
	WWDG_BASE(WWDG_CFR) |= ((uint32_t)base) << 7;
}

static void set_timer_window(uint8_t window)
{
	WWDG_BASE(WWDG_CFR) &= ~WWDG_WINDOW_MASK;
	WWDG_BASE(WWDG_CFR) |= ((uint32_t)window) & WWDG_WINDOW_MASK;
}

static void write_reset_value(void)
{
	WWDG_BASE(WWDG_CFR) = reset_value;
}

void config_window_dog(wwd_configs_t config)
{
	enable_peripheral_clock(WWDG_EN);
	acknowledge_early_wakeup();

	if (config.early_wakeup)
	{
		set_ewi();
	}
	set_timer_base(config.div);
	set_timer_window(config.window);
	window = config.window & WWDG_WINDOW_MASK;
	reset_value = BIT7 | config.reset_value;
}

void enable_window_dog(void)
{
	write_reset_value();
}

void enable_window_dog_kicking(void)
{
	allow_dog_kicking = true;
}

void disable_window_dog_kicking(void)
{
	allow_dog_kicking = false;
}

void try_kicking_window_dog(void)
{
	uint8_t currentValue;

	if (!allow_dog_kicking)
	{
		return;
	}

	currentValue = WWDG_BASE(WWDG_CR);

	if ((currentValue & WWDG_VALUE_MASK) < window && currentValue >= WWDG_RESET_VALUE)
	{
		write_reset_value();
	}
}

bool check_window_early_wakeup(void)
{
	return (WWDG_BASE(WWDG_CR) & BIT0) != 0;
}

void acknowledge_early_wakeup(void)
{
	WWDG_BASE(WWDG_CR) = 0;
}

__inline void kick_the_window_dog(void)
{
	if (!allow_dog_kicking)
	{
		return;
	}
	WWDG_BASE(WWDG_CFR) = reset_value;
}

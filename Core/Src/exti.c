/*
 * exti.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include <assert.h>
#include "exti.h"
#include "sysconfig.h"

#define EXTI_VALID_MASK 0x7FFFFF
#define EXTI_EDGE_REG_MASK 0x77FFFF
#define EXTI_CHANNELS 23
#define PINS_PER_BANK 16

#define EXTI_BASE(n) *(((uint32_t *)0x40013C00) + n)
#define EXTI_IMR 0
#define EXTI_EMR 1
#define EXTI_RTSR 2
#define EXTI_FTSR 3
#define EXTI_SWIER 4
#define EXTI_PR 5

static void set_exti_int_mask(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_IMR) |= mask;
}

static void clear_exti_int_mask(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_IMR) &= ~mask;
}

static uint32_t read_exti_int_mask(void)
{
	return EXTI_BASE(EXTI_IMR);
}

static void set_exti_event_mask(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_EMR) |= mask;
}

static void clear_exti_event_mask(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_EMR) &= ~mask;
}

static uint32_t read_exti_event_mask(void)
{
	return EXTI_BASE(EXTI_EMR);
}

static void set_exti_rising_edge(uint32_t mask)
{
	mask &= EXTI_EDGE_REG_MASK;
	EXTI_BASE(EXTI_RTSR) |= mask;
}

static void clear_exti_rising_edge(uint32_t mask)
{
	mask &= EXTI_EDGE_REG_MASK;
	EXTI_BASE(EXTI_RTSR) &= ~mask;
}

static uint32_t read_exti_rising_edge(void)
{
	return EXTI_BASE(EXTI_RTSR);
}

static void set_exti_falling_edge(uint32_t mask)
{
	mask &= EXTI_EDGE_REG_MASK;
	EXTI_BASE(EXTI_FTSR) |= mask;
}

static void clear_exti_falling_edge(uint32_t mask)
{
	mask &= EXTI_EDGE_REG_MASK;
	EXTI_BASE(EXTI_FTSR) &= ~mask;
}

static uint32_t read_exti_falling_edge(void)
{
	return EXTI_BASE(EXTI_FTSR);
}

static void set_exti_software_int_event(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_SWIER) |= mask;
}

static uint32_t read_exti_software_int_event(void)
{
	return EXTI_BASE(EXTI_SWIER);
}

static uint32_t read_exti_pending(void)
{
	return EXTI_BASE(EXTI_PR);
}

static void clear_exti_pending(uint32_t mask)
{
	mask &= EXTI_VALID_MASK;
	EXTI_BASE(EXTI_PR) = mask;
}

static void configure_exti_channel(bool unmask, void (*callback_unmask)(uint32_t), void (*callback_mask)(uint32_t), uint32_t mask)
{
	if (unmask)
	{
		callback_unmask(mask);
	}
	else
	{
		callback_mask(mask);
	}
}

static void config_gpio_exti(exti_config_t config)
{
	uint32_t mask = 0;
	start_sysconfig();
	configure_exti_line(config.exti_gpio);
	SET_BIT(mask, config.exti_gpio.pin);

	configure_exti_channel(config.unmask_int, set_exti_int_mask, clear_exti_int_mask, mask);
	configure_exti_channel(config.unmask_event, set_exti_event_mask, clear_exti_event_mask, mask);
	configure_exti_channel(config.rising_edge, set_exti_rising_edge, clear_exti_rising_edge, mask);
	configure_exti_channel(config.falling_edge, set_exti_falling_edge, clear_exti_falling_edge, mask);
}

static void config_special_exti(exti_config_t config)
{
	uint32_t mask = 0;

	SET_BIT(mask, config.exti_line);

	configure_exti_channel(config.unmask_int, set_exti_int_mask, clear_exti_int_mask, mask);
	configure_exti_channel(config.unmask_event, set_exti_event_mask, clear_exti_event_mask, mask);
	configure_exti_channel(config.rising_edge, set_exti_rising_edge, clear_exti_rising_edge, mask);
	configure_exti_channel(config.falling_edge, set_exti_falling_edge, clear_exti_falling_edge, mask);
}

void config_exti(exti_config_t config)
{
	if (config.exti_line >= PINS_PER_BANK)
	{
		config_special_exti(config);
	}
	else
	{
		config_gpio_exti(config);
	}
}

void get_exti_configs(uint8_t channel, exti_config_t *config)
{
	uint32_t mask = 0;

	assert(channel < EXTI_CHANNELS);

	if (channel < PINS_PER_BANK)
	{
		get_exti_line_config(channel, &(config->exti_gpio));
	}
	else
	{
		config->exti_line = channel;
	}

	SET_BIT(mask, channel);

	config->unmask_int = (read_exti_int_mask() & mask) != 0;
	config->unmask_event = (read_exti_event_mask() & mask) != 0;
	config->rising_edge = (read_exti_rising_edge() & mask) != 0;
	config->falling_edge = (read_exti_falling_edge() & mask) != 0;
}

void generate_exti_software_event(uint8_t channel)
{
	uint32_t mask = 0;
	assert(channel < EXTI_CHANNELS);
	SET_BIT(mask, channel);
	set_exti_software_int_event(channel);
}

bool exti_software_event_generated(uint8_t channel)
{
	uint32_t mask = 0;
	assert(channel < EXTI_CHANNELS);
	SET_BIT(mask, channel);
	return (read_exti_software_int_event() & mask) != 0;
}

bool check_exti_channel_pending(uint8_t channel)
{
	uint32_t mask = 0;
	assert(channel < EXTI_CHANNELS);
	SET_BIT(mask, channel);
	return (read_exti_pending() & mask) != 0;
}

void acknowledge_exti_event(uint8_t channel)
{
	uint32_t mask = 0;
	assert(channel < EXTI_CHANNELS);
	SET_BIT(mask, channel);
	clear_exti_pending(mask);
}

static uint32_t generate_channel_mask(uint8_t channel)
{
	uint32_t mask = 0;
	if (channel < EXTI_CHANNELS)
	{
		SET_BIT(mask, channel);
	}
	return mask;
}

void acknowledge_multiple_exti_events(uint8_t channel0, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t channel4, uint8_t channel5)
{
	uint32_t mask = 0;

	mask |= generate_channel_mask(channel0);
	mask |= generate_channel_mask(channel1);
	mask |= generate_channel_mask(channel2);
	mask |= generate_channel_mask(channel3);
	mask |= generate_channel_mask(channel4);
	mask |= generate_channel_mask(channel5);

	clear_exti_pending(mask);
}

void clear_pending_exti_events(void)
{
	clear_exti_pending(EXTI_VALID_MASK);
}

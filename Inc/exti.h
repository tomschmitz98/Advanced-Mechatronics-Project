/*
 * exti.h
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#ifndef EXTI_H_
#define EXTI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "stm_utils.h"
#include "sysconfig.h"

#define UNUSED_CHANNEL 32

typedef struct {
	union {
		exti_select_t exti_gpio;
		uint8_t exti_line;
	};
	bool unmask_int;
	bool unmask_event;
	bool rising_edge;
	bool falling_edge;
} exti_config_t;

void config_exti(exti_config_t config);
void get_exti_configs(uint8_t channel, exti_config_t *config);
void generate_exti_software_event(uint8_t channel);
bool exti_software_event_generated(uint8_t channel);
bool check_exti_channel_pending(uint8_t channel);
void acknowledge_exti_event(uint8_t channel);
void acknowledge_multiple_exti_events(uint8_t channel0, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t channel4, uint8_t channel5);
void clear_pending_exti_events(void);

#endif /* EXTI_H_ */

/*
 * sysconfig.h
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include "stm_utils.h"
#include <stdint.h>

typedef struct {
    uint8_t pin;
    gpio_bank_t bank;
} exti_select_t;

// TODO: Add more sysconfig stuff
void start_sysconfig(void);
void configure_exti_line(exti_select_t config);
void get_exti_line_config(uint8_t channel, exti_select_t *gpio_config);

#endif /* SYSCONFIG_H_ */

/*
 * interrupt.h
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <stdint.h>
#include "stm_utils.h"

#define EVENT_TIM3_OVERFLOW BIT0
#define EVENT_EXTI_6 BIT1
#define EVENT_VALID_MASK (EVENT_TIM3_OVERFLOW | EVENT_EXTI_6)

void config_interrupts(void);

extern volatile uint64_t gEvents;

#endif /* INTERRUPT_H_ */

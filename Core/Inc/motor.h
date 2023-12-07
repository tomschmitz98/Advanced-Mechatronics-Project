/*
 * motor.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdbool.h>

void init_motor_pins(void);
void start_slap(void);
void reset_slap(void);
bool done_with_actuation(void);
void block_actuation_events(void);

#endif /* INC_MOTOR_H_ */

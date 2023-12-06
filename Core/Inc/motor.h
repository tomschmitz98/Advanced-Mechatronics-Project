/*
 * motor.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdint.h>

void pwm(int16_t val);

void init_motor(void);
void start_slap(void);
void slap(void);
void reset_motor(void);

#endif /* INC_MOTOR_H_ */

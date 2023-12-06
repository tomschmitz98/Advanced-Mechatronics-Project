/*
 * motor.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdint.h>

void init_motor(void);
void start_slap(void);
void slap(void);
void reset_motor(void);
void print_angle(void);
void pwm(int16_t val);

#endif /* INC_MOTOR_H_ */

/*
 * timers.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

void init_heartbeat(void);
void kick_the_watchdog(void);
void disable_watchdog(void);
void enable_watchdog(void);
uint32_t read_heartbeat(void);
void start_measurement(void);
void stop_measurement(void);
uint32_t read_measurement(void);
uint32_t current_ts(void);

void init_motor_timer(void);
void init_pid_timer(void);

#endif /* TIMERS_H_ */

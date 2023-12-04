/*
 * timers.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef TIMERS_H_
#define TIMERS_H_

void init_heartbeat(void);
void kick_the_watchdog(void);
void disable_watchdog(void);
void enable_watchdog(void);

#endif /* TIMERS_H_ */

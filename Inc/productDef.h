/*
 * productDef.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef PRODUCTDEF_H_
#define PRODUCTDEF_H_

#include <stdint.h>

#define E_NO_EVENT 0x00000000
#define E_HEARTBEAT 0x00000001

#define E_VALID_MASK (E_HEARTBEAT)

extern volatile uint32_t gEvents;

#endif /* PRODUCTDEF_H_ */

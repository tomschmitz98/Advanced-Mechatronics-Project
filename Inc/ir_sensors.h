/*
 * ir_sensors.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef IR_SENSORS_H_
#define IR_SENSORS_H_

#include <stdbool.h>

typedef enum {
	IR_0 = 0,
	IR_1 = 1,
	IR_2 = 2,
	IR_3 = 3,
	IR_4 = 4
} ir_sensor_t;

void initialize_ir_sensors(void);
void refresh_ir_sensors(void);
bool all_ir_sensors_covered(void);
bool read_ir_sensor(ir_sensor_t sensor);

#endif /* IR_SENSORS_H_ */

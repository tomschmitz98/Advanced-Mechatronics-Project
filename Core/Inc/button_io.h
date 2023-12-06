/*
 * button_io.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#ifndef INC_BUTTON_IO_H_
#define INC_BUTTON_IO_H_

#include <stdbool.h>

typedef enum { PAUSE_BUTTON = 0, START_BUTTON = 1 } button_t;

void init_buttons(void);
bool read_button(button_t btn);
bool button_changed_state(button_t btn);

#endif /* INC_BUTTON_IO_H_ */

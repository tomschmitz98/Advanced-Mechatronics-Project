/*
 * reaction.h
 *
 *  Created on: Dec 4, 2023
 *      Author: Tom
 */

#ifndef REACTION_H_
#define REACTION_H_

#include <stdint.h>

void config_reaction(void);
void start_reaction(void);
void stop_reaction(void);
uint32_t read_reaction(void);

#endif /* REACTION_H_ */

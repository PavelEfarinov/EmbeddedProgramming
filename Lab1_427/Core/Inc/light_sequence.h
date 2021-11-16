/*
 * light_sequence.h
 *
 *  Created on: Nov 16, 2021
 *      Author: efarinov
 */

#ifndef INC_LIGHT_SEQUENCE_H_
#define INC_LIGHT_SEQUENCE_H_


typedef struct light_sequence {
	int period;
	int sequence_size;
	int last_tick;
	enum LED_STATE *states;
} light_sequence_t;


#endif /* INC_LIGHT_SEQUENCE_H_ */

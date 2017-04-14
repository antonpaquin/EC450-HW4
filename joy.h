/*
 * joy.h
 *
 *  Created on: Apr 14, 2017
 *      Author: anton
 */

#ifndef JOY_H_
#define JOY_H_

#include "msp.h"

typedef struct {} joy_t;
typedef struct {
    uint16_t x;
    uint16_t y;
} joy_pos;

joy_t* joy_init();
joy_pos joy_read();


#endif /* JOY_H_ */

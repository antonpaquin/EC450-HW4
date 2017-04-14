/*
 * transfer.h
 *
 *  Created on: Apr 14, 2017
 *      Author: anton
 */

#ifndef TRANSFER_H_
#define TRANSFER_H_

#include "msp.h"
#include "spi.h"

typedef struct {
    uint16_t x;
    uint16_t y;
} joy_data_t;

void tr_send(spi_t* spi, joy_data_t dat);
void tr_recv(char c);
void tr_callback(void(*)(joy_data_t));

#endif /* TRANSFER_H_ */

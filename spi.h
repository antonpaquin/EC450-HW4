/*
 * spi.h
 *
 *  Created on: Apr 13, 2017
 *      Author: anton
 */

#ifndef SPI_H_
#define SPI_H_

#include "gpio.h"
#include "msp.h"

#define SPI_MASTER 1
#define SPI_SLAVE 0
#define spi_buffer_size 100

typedef struct {} spi_t;

spi_t* spi_init(void* mem, char isMaster,
                gpio_t* mosi, gpio_t* miso, gpio_t* clk, gpio_t* sync,
                Timer_A_Type* timer, IRQn_Type interruptFlag);
#define spi_size ( \
    sizeof(void(*)(char)) + \
    4 * sizeof(gpio_t*) + \
    8 * sizeof(char) + \
    spi_buffer_size * sizeof(char) + \
    sizeof(Timer_A_Type*) \
)

void spi_onInterrupt(spi_t* self);
void spi_setspeed(spi_t* self, uint16_t speed);

void spi_recvByte(spi_t* self, void(*callback)(char));
void spi_put(spi_t* self, char data);

void spi_hold(spi_t* self);
void spi_start(spi_t* self);

#endif /* SPI_H_ */

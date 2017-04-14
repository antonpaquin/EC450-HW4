/*
 * gpio.h
 *
 *  Created on: Apr 13, 2017
 *      Author: anton
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "msp.h"
#include <stdlib.h>

typedef struct {} gpio_t;
#define gpio_t_size (3*(sizeof(uint8_t*)) + sizeof(uint8_t))

#define new_gpio(port, pin) gpio_init(malloc(gpio_t_size), port, pin)

gpio_t* gpio_init(void* mem, char port, char mask);

void gpio_input(gpio_t* gpio);
void gpio_output(gpio_t* gpio);

char gpio_read(gpio_t* gpio);

void gpio_on(gpio_t* gpio);
void gpio_off(gpio_t* gpio);
void gpio_toggle(gpio_t* gpio);


#endif /* GPIO_H_ */

/*
 * gpio.c
 *
 *  Created on: Apr 13, 2017
 *      Author: anton
 */

#include "gpio.h"

typedef struct {
    volatile uint8_t* in;
    volatile uint8_t* out;
    volatile uint8_t* dir;
    uint8_t mask;
} gpio_tx;

gpio_t* gpio_init(void* mem, char port, char bit) {
    gpio_tx* gpio = (gpio_tx*) mem;

    switch(port) {
        case 1:
            gpio -> in = (volatile uint8_t*) &(P1 -> IN);
            gpio -> out = &(P1 -> OUT);
            gpio -> dir = &(P1 -> DIR);
            break;
        case 2:
            gpio -> in = (volatile uint8_t*) &(P2 -> IN);
            gpio -> out = &(P2 -> OUT);
            gpio -> dir = &(P2 -> DIR);
            break;
        case 3:
            gpio -> in = (volatile uint8_t*) &(P3 -> IN);
            gpio -> out = &(P3 -> OUT);
            gpio -> dir = &(P3 -> DIR);
            break;
        case 4:
            gpio -> in = (volatile uint8_t*) &(P4 -> IN);
            gpio -> out = &(P4 -> OUT);
            gpio -> dir = &(P4 -> DIR);
            break;
        case 5:
            gpio -> in = (volatile uint8_t*) &(P5 -> IN);
            gpio -> out = &(P5 -> OUT);
            gpio -> dir = &(P5 -> DIR);
            break;
        case 6:
            gpio -> in = (volatile uint8_t*) &(P6 -> IN);
            gpio -> out = &(P6 -> OUT);
            gpio -> dir = &(P6 -> DIR);
            break;
        case 7:
            gpio -> in = (volatile uint8_t*) &(P7 -> IN);
            gpio -> out = &(P7 -> OUT);
            gpio -> dir = &(P7 -> DIR);
            break;
        case 8:
            gpio -> in = (volatile uint8_t*) &(P8 -> IN);
            gpio -> out = &(P8 -> OUT);
            gpio -> dir = &(P8 -> DIR);
            break;
    }

    gpio -> mask = (1 << bit);
    return (gpio_t*) gpio;
}

void gpio_input(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    *(g -> dir) &= ~(g -> mask);

}
void gpio_output(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    *(g -> dir) |= g -> mask;
}

char gpio_read(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    if ((*(g -> in) & (g -> mask)) != 0) {
        return 1;
    } else {
        return 0;
    }
}

void gpio_on(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    *(g -> out) |= g -> mask;
}
void gpio_off(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    *(g -> out) &= ~(g -> mask);
}
void gpio_toggle(gpio_t* gpio) {
    gpio_tx* g = (gpio_tx*) gpio;
    *(g -> out) ^= (g -> mask);
}

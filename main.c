//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include <stdlib.h>
#include <stdio.h>

#include "spi.h"
#include "gpio.h"

spi_t* conn;
char buff[256];
char buffpos = 0;

#define MASTER
//#define SLAVE

void onGetByte(char);

void main(void) {
	
    WDTCTL = WDTPW | WDTHOLD;
    __enable_interrupt();

    CS -> KEY |=  0x695A;
    CS -> CTL1 &= ~(BIT(10)); CS -> CTL1 &= ~BIT9; CS -> CTL1 |= BIT8;


    gpio_t* mosi = new_gpio(4,5);
    gpio_t* miso = new_gpio(4,4);
    gpio_t* clk = new_gpio(5,4);
    gpio_t* sync = new_gpio(5,5);

#ifdef MASTER
    conn = spi_init(malloc(spi_size), SPI_MASTER, mosi, miso, clk, sync, TIMER_A0, TA0_N_IRQn);
    spi_setspeed(conn, 1000);
    spi_put(conn, 179);
    spi_put(conn, 123);
    spi_put(conn, 31);
    spi_put(conn, 252);
    spi_put(conn, 208);
#endif
#ifdef SLAVE
    conn = spi_init(malloc(spi_size), SPI_SLAVE, mosi, miso, clk, sync, TIMER_A0, TA0_N_IRQn);
    spi_setspeed(conn, 100);
    spi_put(conn, 255);
    spi_put(conn, 127);
    spi_put(conn, 127);
    spi_put(conn, 127);
    spi_put(conn, 0);
    spi_put(conn, 0);
    spi_put(conn, 0);
    spi_put(conn, 0);
    spi_put(conn, 179);
    spi_put(conn, 31);
    spi_put(conn, 170);
    spi_put(conn, 179);
    spi_put(conn, 0);
    spi_put(conn, 0);
    spi_put(conn, 0);
    spi_put(conn, 0);
#endif
    spi_recvByte(conn, onGetByte);
    spi_start(conn);

    while (1) {
        __sleep();

    }
}

void onGetByte(char c) {
    buff[buffpos] = c;
    buffpos++;
    if (buffpos == 255) {
        buffpos = 0;
    }
}

void TA0_N_IRQHandler(void) {
    spi_onInterrupt(conn);
}


/*
 * spi.c
 *
 *  Created on: Apr 4, 2017
 *      Author: anton
 */

#include "msp.h"
#include "spi.h"

#if !defined(SPI_MASTER) && !defined(SPI_SLAVE)
#error "You should define SPI_MASTER or SPI_SLAVE to use this"
#endif

#define SPI_BUFSIZE 100

char spi_writebuf[SPI_BUFSIZE];
char spi_writeSize;

char spi_readbuf[SPI_BUFSIZE];
int spi_rpos;

char spi_readOut[SPI_BUFSIZE];
char spi_readSize;

char spi_writeOut[SPI_BUFSIZE];
int spi_wpos;

char spi_timer;
char spi_clkEdge;
char spi_readEn;
char spi_writeEn;

#ifdef SPI_SLAVE
char spi_synchronized;
#endif

void spi_bitWrite();
void spi_bitRead();

void spi_setup() {
    char i;
    for (i=0; i<SPI_BUFSIZE; i++) {
        spi_writebuf[i] = 0;
        spi_readbuf[i] = 0;
        spi_readOut[i] = 0;
        spi_writeOut[i] = 0;
    }
    spi_wpos = 0;
    spi_rpos = 0;
    spi_readSize = 0;
    spi_writeSize = 0;

    spi_clkEdge = 0;
    spi_timer = 0;

    // Run off ACLK
    TIMER_A0 -> CTL &= ~BIT9; TIMER_A0 -> CTL |= BIT8;
    TIMER_A0 -> CTL |= BIT7; TIMER_A0 -> CTL |= BIT6;
    // Turn on counting up
    TIMER_A0 -> CTL &= ~BIT5; TIMER_A0 -> CTL |= BIT4;
    // Ping at a constant 32768/2048
    TIMER_A0 -> CCR[0] = 10;

    // Enable interrupts
    TIMER_A0 -> CTL |= BIT1;
    NVIC -> ISER[0] = 1 << ((TA0_N_IRQn) & 31);

#ifdef SPI_SLAVE
    P4 -> DIR &= ~BIT5;
    P4 -> DIR |= BIT7;
    P5 -> DIR &= ~BIT4;
    P5 -> DIR &= ~BIT5;
#endif
#ifdef SPI_MASTER
    P4 -> DIR |= BIT5;
    P4 -> DIR &= ~BIT7; //MISO
    P5 -> DIR |= BIT4; // MOSI
    P5 -> DIR |= BIT5; // CLK
#endif

#ifdef SPI_SLAVE
    spi_synchronized = 0;
#endif
}



char* spi_read(char bytes) {
    char i;
    spi_readEn = 1;
    while (bytes * 8 > spi_rpos) {
        __sleep();
    }
    for (i=0; i<bytes; i++) {
        spi_readOut[i] = spi_readbuf[i];
    }
    spi_rpos = 0;
    spi_readEn = 0;
#ifdef SPI_SLAVE
    spi_synchronized = 0;
#endif
    return spi_readOut;
}

void spi_put(char c) {
    spi_writebuf[spi_writeSize] = c;
    spi_writeSize += 1;
}

void spi_writeout() {
    char i;
    spi_writeEn = 1;
    for (i=0; i<spi_writeSize; i++) {
        spi_writeOut[i] = spi_writebuf[i];
    }
}

char spi_writeFinished() {
    return (spi_writeEn == 0);
}

void spi_bitWrite() {
    char bit = 1 << (spi_wpos % 8);
    char byte = spi_wpos / 8;
    char val = spi_writeOut[byte] & bit;
    spi_wpos++;
#ifdef SPI_MASTER
    if (val == 0) {
        P5 -> OUT &= ~BIT4;
    } else {
        P5 -> OUT |= BIT4;
    }
    if (bit == 1) {
        P4 -> OUT |= BIT5;
    } else {
        P4 -> OUT &= ~BIT5;
    }
#endif
#ifdef SPI_SLAVE
    if (val == 0) {
        P4 -> OUT &= ~BIT7;
    } else {
        P4 -> OUT |= BIT7;
    }
#endif
    if (spi_wpos == 8 * spi_writeSize) {
        spi_wpos = 0;
        spi_writeEn = 0;
        spi_writeSize = 0;
    }
}

void spi_bitRead() {
    char bit = 1 << (spi_rpos % 8);
    char byte = spi_rpos / 8;
    spi_rpos++;
#ifdef SPI_MASTER
    char val = P4 -> IN & BIT7;
#endif
#ifdef SPI_SLAVE
    char val = P5 -> IN & BIT4;
#endif
    if (val == 0) {
        spi_readbuf[byte] &= ~bit;
    } else {
        spi_readbuf[byte] |= bit;
    }
}


void spi_onTimingInterrupt() {
#ifdef SPI_MASTER
    if (spi_timer % 8 == 0) {
        P5 -> OUT ^= BIT5;
    }
#endif
#ifdef SPI_SLAVE
    if (spi_synchronized == 0) {
        if (((P4 -> IN) & BIT5) == 0) {
            return;
        } else {
            spi_synchronized = 1;
        }
    }
#endif
    if (((P5 -> IN) & BIT5) != spi_clkEdge) {
        spi_clkEdge = (P5 -> IN) & BIT5;
        if (spi_readEn != 0 && spi_clkEdge != 0) { // on posedge read
            spi_bitRead();
        }
        if (spi_writeEn != 0 && spi_clkEdge == 0) { // on negedge write
            spi_bitWrite();
        }
    }
    spi_timer += 1;
}

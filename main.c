//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "spi.h"

void main(void) {
	
    WDTCTL = WDTPW | WDTHOLD;
    __enable_interrupt();


    spi_setup();

#ifdef SPI_MASTER
    spi_put(189);
    spi_writeout();
    while (! spi_writeFinished() ) {
        __sleep();
    }
    spi_put(12);
    spi_writeout();
    while (! spi_writeFinished() ) {
        __sleep();
    }
    spi_put(128);
    spi_put(43);
    spi_put(170);
    spi_writeout();
    while (! spi_writeFinished() ) {
        __sleep();
    }
    char* c = spi_read(1);
    printf("Vaue is %d\n", *c);
#endif
#ifdef SPI_SLAVE
    while(1) {
        char* c = spi_read(1);
        spi_put(200);
        spi_writeout();
        printf("Value is %d\n", *c);
    }
#endif

    while (1) {
        __sleep();
    }
}

void TA0_N_IRQHandler(void) {
    TIMER_A0 -> CTL &= ~BIT0;
    spi_onTimingInterrupt();
}


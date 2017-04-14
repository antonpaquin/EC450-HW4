/*
 * spi.c
 *
 *  Created on: Apr 13, 2017
 *      Author: anton
 */


#include "spi.h"

typedef struct {\

    void(*callback)(char);
    gpio_t* mosi;
    gpio_t* miso;
    gpio_t* sync;
    gpio_t* clk;
    char isMaster;
    Timer_A_Type* timer;

    char outbuffer[spi_buffer_size];
    char outbuffer_index;
    char out_bytepos;

    char inbuffer;
    char in_bytepos;

    char enabled;
    char clkstate;
    char writestate;
} spi_tx;

void spi_noop(char x) {

};
void spi_master_tick(spi_t* self);
void spi_slave_tick(spi_t* self);
inline char spi_readbit(char byte, char pos);
inline void spi_setbit(char* byte, char pos, char value);
void spi_rotoutbuffer(spi_tx* self);

spi_t* spi_init(void* mem, char isMaster,
                gpio_t* mosi, gpio_t* miso, gpio_t* clk, gpio_t* sync,
                Timer_A_Type* timer, IRQn_Type interruptFlag) {

    spi_tx* spi = (spi_tx*) mem;

    spi -> isMaster = isMaster;
    if (isMaster) {
        gpio_input(miso);
        gpio_output(clk);
        gpio_output(mosi);
        gpio_output(sync);
    } else {
        gpio_output(miso);
        gpio_input(clk);
        gpio_input(mosi);
        gpio_input(sync);
    }

    gpio_off(miso);
    gpio_off(clk);
    gpio_off(mosi);
    gpio_off(sync);

    spi -> callback = spi_noop;
    spi -> mosi = mosi;
    spi -> miso = miso;
    spi -> sync = sync;
    spi -> clk = clk;

    spi -> timer = timer;
    timer -> CTL &= ~BIT9; timer -> CTL |= BIT8; // ACLK
    timer -> CTL &= ~BIT7; timer -> CTL &= ~BIT6; // Div 1
    timer -> CTL &= ~BIT5; timer -> CTL |= BIT4; // Count-up mode
    timer -> CTL |= BIT1; // Interrupt on
    NVIC -> ISER[0] = 1 << ((interruptFlag) & 31);

    char i;
    for (i=0; i<spi_buffer_size; i++) {
        (spi -> outbuffer)[i] = 0;
    }
    spi -> outbuffer_index = 0;
    spi -> out_bytepos = 0;

    spi -> inbuffer = 0;
    spi -> in_bytepos = 0;

    spi -> enabled = 0;
    spi -> clkstate = 0;
    spi -> writestate = 0;

    return (spi_t*) spi;
}

void spi_recvByte(spi_t* self, void(*callback)(char)) {
    spi_tx* s = (spi_tx*) self;
    s -> callback = callback;
}

void spi_put(spi_t* self, char data) {
    spi_tx* s = (spi_tx*) self;
    (s -> outbuffer)[(s -> outbuffer_index)] = data;
    s -> outbuffer_index += 1;
}

void spi_hold(spi_t* self) {
    spi_tx* s = (spi_tx*) self;
    s -> enabled = 0;
}

void spi_start(spi_t* self) {
    spi_tx* s = (spi_tx*) self;
    s -> enabled = 1;
}

void spi_onInterrupt(spi_t* self) {
    spi_tx* s = (spi_tx*) self;
    s -> timer -> CTL &= ~BIT0;
    if (s -> isMaster) {
        spi_master_tick(self);
    } else {
        spi_slave_tick(self);
    }
}

void spi_setspeed(spi_t* self, uint16_t speed) {
    spi_tx* s = (spi_tx*) self;
    s -> timer -> CCR[0] = speed;
}

void spi_master_tick(spi_t* self) {
    spi_tx* s = (spi_tx*) self;

    if (s -> enabled) {
        gpio_toggle(s -> clk);
        s -> clkstate ^= BIT0;
    } else {
        gpio_off(s -> clk);
        s -> clkstate = 0;
        return;
    }

    if (s -> clkstate == 0) {
        spi_setbit(&(s -> inbuffer), s -> in_bytepos, gpio_read(s -> miso)); // Read a bit
        s -> in_bytepos += 1;
        if (s -> in_bytepos == 8) { // If we've got a full byte, run the callback & reset
            s -> in_bytepos = 0;
            s -> callback(s -> inbuffer);
            s -> inbuffer = 0;
        }
    } else {
        if (s -> out_bytepos == 0) { // Set the sync line high for the first bit
            gpio_on(s -> sync);
        } else {
            gpio_off(s -> sync);
        }

        if (spi_readbit(s -> outbuffer[0], s -> out_bytepos) == 0) {
            gpio_off(s -> mosi);
        } else {
            gpio_on(s -> mosi);
        }
        s -> out_bytepos += 1;

        if (s -> out_bytepos == 8) {
            if (s -> outbuffer_index > 0) {
                char i;
                for (i=0; i<spi_buffer_size-1; i++) {
                    s -> outbuffer[i] = s -> outbuffer[i+1];
                }
                s -> outbuffer[spi_buffer_size-1] = 0;
                s -> outbuffer_index -= 1;
            } else {
                s -> outbuffer[0] = 0;
            }
            s -> out_bytepos = 0;
        }
    }
}

void spi_slave_tick(spi_t* self) {
    spi_tx* s = (spi_tx*) self;

    if (! (s -> enabled)) {
        gpio_off(s -> miso);
        return;
    }

    char clk = gpio_read(s -> clk);
    if (clk == s -> clkstate) {
        return;
    } else {
        s -> clkstate = clk;
    }

    if (clk == 0 && ((s -> in_bytepos > 0) || (gpio_read(s -> sync) != 0))) { // If it's read time, and we're either already reading or synced to read
        spi_setbit(&(s -> inbuffer), s -> in_bytepos, gpio_read(s -> mosi));
        s -> in_bytepos += 1;
        s -> writestate = 1;

        if (s -> in_bytepos == 8) {
            s -> in_bytepos = 0;
            s -> callback(s -> inbuffer);
            s -> inbuffer = 0;
        }

    } else {
        if ((s -> writestate) || (s -> out_bytepos > 0)) {
            s -> out_bytepos += 1;
        }

        if (s -> out_bytepos == 8) {
            if (s -> outbuffer_index > 0) {
                char i;
                for (i=0; i<spi_buffer_size-1; i++) {
                    s -> outbuffer[i] = s -> outbuffer[i+1];
                }
                s -> outbuffer[spi_buffer_size-1] = 0;
                s -> outbuffer_index -= 1;
            } else {
                s -> outbuffer[0] = 0;
            }
            s -> out_bytepos = 0;
            s -> writestate = 0;
        }

        if (spi_readbit(s -> outbuffer[0], s -> out_bytepos) != 0) {
            gpio_on(s -> miso);
        } else {
            gpio_off(s -> miso);
        }
    }
}

inline char spi_readbit(char byte, char pos) {
    if ((byte & (1 << pos)) == 0) {
        return 0;
    } else {
        return 1;
    }
}

inline void spi_setbit(char* byte, char pos, char value) {
    if (value == 0) {
        *byte &= ~(1 << pos);
    } else {
        *byte |= (1 << pos);
    }
}

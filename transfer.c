/*
 * transfer.c
 *
 *  Created on: Apr 14, 2017
 *      Author: anton
 */

#include "msp.h"
#include "transfer.h"
#include "spi.h"

#define data_start 128
#define data_end 136

void(*tr_cb_x)(joy_data_t);
char buff[6];
char buffpos = 0;

void tr_send(spi_t* spi, joy_data_t dat) {
    spi_put(spi, data_start);
    spi_put(spi, (char) (dat.x & 127)); // [6:0]
    spi_put(spi, (char) ((dat.x & 16256) >> 7)); // [14:7]
    spi_put(spi, (char) (dat.y & 127));
    spi_put(spi, (char) ((dat.y & 16256) >> 7));
    spi_put(spi, data_end);
}

void tr_recv(char c) {
    if (buffpos == 0 && c != data_start) {
        return;
    }

    buff[buffpos] = c;
    buffpos += 1;
    if (buffpos == 6) {
        buffpos = 0;
        if (c == data_end) {
            joy_data_t res;
            res.x = buff[1] & 127;
            res.x += ((buff[2] & 127) << 7);
            res.y = buff[3] & 127;
            res.y += ((buff[4] & 127) << 7);
            char i;
            for (i=0; i<6; i++) {
                buff[i] = 0;
            }
            tr_cb_x(res);
        }
    }
}
void tr_callback(void(*callback)(joy_data_t)) {
    tr_cb_x = callback;
}



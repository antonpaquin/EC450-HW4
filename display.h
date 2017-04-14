/*
 * display.h
 *
 *  Created on: Apr 13, 2017
 *      Author: wchen
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <stdio.h>

#define TEXTCOL GRAPHICS_COLOR_YELLOW
#define BACKCOL GRAPHICS_COLOR_BLACK
#define DOTCOL GRAPHICS_COLOR_LIGHT_GREEN
#define DOTCOL_PRESSED GRAPHICS_COLOR_RED
#define RADIUS 2

extern Graphics_Context g_sContext;
extern uint16_t xscreen, yscreen;      // current screen location coordinates

void put_dot(uint16_t x,uint16_t y, uint32_t dotcolor);
void print_current_results(uint16_t *results);
void init_display();



#endif /* DISPLAY_H_ */

/*
 * joystick.h
 *
 *  Created on: Apr 5, 2017
 *      Author: william
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_


#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <stdio.h>




extern uint16_t resultsBuffer[2];           // latest readings from the analog inputs
extern uint16_t buttonPressed;              // 1 if joystick button is pressed
extern uint16_t print_flag;                 // flag to signal main to redisplay - set by ADC14



void init_WDT();
void init_ADC();
void ADC14_IRQHandler(void);
void WDT_A_IRQHandler(void);



#endif /* JOYSTICK_H_ */

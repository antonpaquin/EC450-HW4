/*
 * display.c
 *
 *  Created on: Apr 13, 2017
 *      Author: wchen
 */




#include "display.h"

 Graphics_Context g_sContext;
 uint16_t xscreen, yscreen;      // current screen location coordinates

void put_dot(uint16_t x,uint16_t y, uint32_t dotcolor){
    // erase previous dot
    Graphics_setForegroundColor(&g_sContext, BACKCOL);
    Graphics_fillCircle(&g_sContext,xscreen,yscreen,RADIUS);
    // draw the requested circle
    Graphics_setForegroundColor(&g_sContext, dotcolor);
    Graphics_fillCircle(&g_sContext,x,y,RADIUS);
    xscreen=x;
    yscreen=y;
}

// text printout of joystick readings on the screen
void print_current_results(uint16_t *results){
    char string[8];

    Graphics_setForegroundColor(&g_sContext, TEXTCOL);
    sprintf(string, "X: %5d", results[0]);
    Graphics_drawString(&g_sContext,(int8_t *)string,8,20,116,OPAQUE_TEXT);
    sprintf(string, "Y: %5d", results[1]);
    Graphics_drawString(&g_sContext,(int8_t *)string,8,76,116,OPAQUE_TEXT);
}

void init_display(){
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,&g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, TEXTCOL);
    Graphics_setBackgroundColor(&g_sContext, BACKCOL);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawString(&g_sContext,"J:",AUTO_STRING_LENGTH,0,116,OPAQUE_TEXT);
    xscreen=0;
    yscreen=0;  // just use origin, first write is a background
}

/**************************************************************************
 * Joystick Example Version 3a (March 2017)
 *
 * This is a (highly) modified version of the EDU Booster Pack
 * Joystick example from the TI SDK.
 *
 * Operation:
 * The application uses the EDU MKII Joystick and Crystalfont display
 * The Joysick is periodically sampled (by the MSP432 ADC14).
 * The resulting x,y values (between 0 and 2**14-1) are rescaled to 7 bits
 * and used as x,y coordinates on the 128x128 display for drawing a small circle.
 * As in the original Joystick example, the ADC14 readings are also written on the screen.
 *
 * Parameters and Details
 * ======================
 *
 * Connections (from the EDU MKII):
 * ------------------------------
 * Joystick X analog input: P6.0 (A15) [A15 is the default tertiary function of P6.0]
 * Joystick Y analog input: P4.4 (A9)
 * Joystick Button input: P4.1 (not used here, yet)
 *
 * The Crystalfont128x128 display is connected to an SPI communication interface
 * and several gpio pins:
 * LCD SPI CLK:  P1.5 (UCB0CLK)
 * LCD RST:      P5.7
 * LCD SPI MOSI: P1.6 (UCB0SIMO)
 * LCD SPI CS:   P5.0
 * LCD RS PIN:   P3.7
 *
 * The display is managed by a Graphics Library (grlib.h) and
 * an LCD driver for the Crystalfont128x128 display.
 * See the graphics driver library documentation in the MSP432 SDK
 * and the LCD driver code that is part of this project.
 *
 * Timing parameters
 * -----------------
 * The MCLK and SMLK are driven by the DCO at 10MHz (still at VCORE0 voltage levels)
 * We use this faster than default speed mainly to be sure that graphics library
 * has enough headroom for CPU utilization (not sure if this is necessary).
 *
 * The ADC14 could be run in automatic mode to take samples repeatedly (this was
 * done in original TI example), but we choose to trigger individual readings rapidly
 * on a human time scale but not on the CPU time scale by using the WDT with divisor 8K.
 *
 * Software Overview:
 *
 * The WDT interrupt triggers a single ADC14 sequence of channels measurement.
 * The completion of the final ADC channel measurement triggers the ADC14 interrupt
 * The ADC interrupt saves the joystick readings (including the button, and
 * sets a global 'print_flag' indicating a refresh of the screen is needed.
 *
 * After any interrupt, the main program wakes up, checks the print flag and,
 * if it is set, refreshes the display:
 * It computes the screen coordinates for the analog measurements:
 *         xscreen = x/128
 *         yscreen = 127-y/128
 * and draws a solid circle whose color depends on whether the joystick button is pressed.
 **************************************************************************************/



#include "joystick.h"

 uint16_t resultsBuffer[2];           // latest readings from the analog inputs
 uint16_t buttonPressed;              // 1 if joystick button is pressed
 uint16_t print_flag;                 // flag to signal main to redisplay - set by ADC14


void WDT_A_IRQHandler(void)
{
    MAP_ADC14_toggleConversionTrigger(); // trigger the next conversion
}

void init_WDT(){
    MAP_WDT_A_initIntervalTimer(WDT_A_CLOCKSOURCE_SMCLK,WDT_A_CLOCKITERATIONS_8192);
    MAP_WDT_A_startTimer(); // start the timer
}

void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    /* ADC_MEM1 conversion completed */
    if(status & ADC_INT1)
    {
        /* Store ADC14 conversion results */
        resultsBuffer[0] = MAP_ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = MAP_ADC14_getResult(ADC_MEM1);

        /* Determine if JoyStick button is pressed */
        buttonPressed = (P4IN & GPIO_PIN1)?0:1;

        print_flag=1;  // signal main to refresh the display
    }
}

void init_ADC(){
    /* Configure Pin 6.0 (A15) and 4.4 (A9) to be analog inputs ('tertiary function') */
    /* see the port 4 and port 6 pinout details of the MSP432p401r */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initializing ADC (ADCOSC/64/8)
     * drive from the internal ASD oscillator
     * with predivider 64 and divider 8, no routing to internal pins
     */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, ADC_NOROUTE);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
     * Basic operation is sequence mode with
     *   ADC-MEM0 -- A15
     *   ADC_MEM1 -- A9
     *
     *   NO automatic repeats
     */
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, false); // use MEM...MEM1 channels
    // configure each memory channel:

    MAP_ADC14_configureConversionMemory(ADC_MEM0,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

    MAP_ADC14_configureConversionMemory(ADC_MEM1,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    /* Enabling the interrupt when a conversion on channel 1 (end of sequence)
     * is complete and enabling conversions
     */
    MAP_ADC14_enableInterrupt(ADC_INT1);

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Enable conversions (must be triggered using ASC14_toggle_conversion()) */
    MAP_ADC14_enableConversion();
    //MAP_ADC14_toggleConversionTrigger();

}


/*
 * Code to test the TivaWare functions which turns on LEDs in order
 *
 *  Created on: 15/09/2021
 *      Author: Ahmed El Daly
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "tm4c123gh6pm.h"

#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

int main(void)
{
    /*Enable clock with divider of 4, use PLL, 16MHz external crystal frequency, main oscillator source*/
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /*Declare PF1:PF3 as outputs*/
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(1)
    {
        /*Turn on the Red LED*/
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, RED);

        /* Delay for a bit */
        SysCtlDelay(2000000);

        /*Turn on the Blue LED*/
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, BLUE);

        /* Delay for a bit */
        SysCtlDelay(2000000);

        /*Turn on the Green LED*/
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GREEN);

        /* Delay for a bit */
        SysCtlDelay(2000000);

    }
}

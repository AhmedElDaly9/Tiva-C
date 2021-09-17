/*
 * GPIO_code.c
 *
 *  Created on: 09/09/2021
 *      Author: Ahmed El Daly
 */


#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

void SystemInit(){}

void PORTF_init(void)
{
    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    /*Polling until the clock is started and GPIO Port F is ready for access*/
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){}

    /*Unlock the GPIO PORT F*/
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;

    /*Allow changes to PF0:PF4 pins*/
    GPIO_PORTF_CR_R |= 0x1F;

    /*Declare PF0 & PF4 as input pins, PF1:PF3 as output pins*/
    GPIO_PORTF_DIR_R = 0x0E;

    /*Disable alternate functions on all pins of PORT F*/
    GPIO_PORTF_AFSEL_R = 0x00;

    /*Enable pull-up resistor for PF0 and PF4 pins*/
    GPIO_PORTF_PUR_R = 0x11;

    /*Enable the digital functions of PF0:PF4*/
    GPIO_PORTF_DEN_R = 0x1F;
}

int main ()
{
    /*Varaible to carry the switches inputs*/
    unsigned long switches;

    /*Initialize PF0 & PF4 for switches and PF1:PF3 for LEDs*/
    PORTF_init();

    while(1)
    {
        /*Read the inputs of the switches pins PF0 and PF4*/
        switches = GPIO_PORTF_DATA_R & 0x11;

        /*Based on negative logic switches write the required output on PF1:PF3*/
        switch (switches)
        {
            /*Both SW1 & SW2 are pressed*/
            case 0x00:
                /*Turn on only the green LED*/
                GPIO_PORTF_DATA_R = GREEN;
                break;

            /*SW1 pressed & SW released*/
            case 0x01:
                /*Turn on only the blue LED*/
                GPIO_PORTF_DATA_R = BLUE;
                break;

            /*SW1 released & SW pressed*/
            case 0x10:
                /*Turn on only the red LED*/
                GPIO_PORTF_DATA_R = RED;
                break;

            /*Both SW1 & SW2 are released*/
            default:
                /*Turn off all the lights*/
                GPIO_PORTF_DATA_R = 0;
                break;
        }
    }
}

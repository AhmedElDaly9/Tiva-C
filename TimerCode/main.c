/*
 * Turn on LEDs in order with 3 seconds periods
 *
 *  Created on: 11/09/2021
 *      Author: Ahmed El Daly
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

void SystemInit(){}

void periodicTimer0Init(void)
{
    /*Enable and provide a clock to 16/32-bit general-purpose timer module 0 in Run mode*/
    SYSCTL_RCGCTIMER_R |= 0x01;

    /*Disable the Timer0 before the configurations*/
    TIMER0_CTL_R &= (~(0x01));

    /*32-bit timer configuration*/
    TIMER0_CFG_R &= (~(0x07));

    /*TimerA mode: Periodic timer mode, down counter*/
    TIMER0_TAMR_R &= ((~(0x07)) | (0x02));

    /*Start counting from 3 seconds with 16 MHz frequency*/
    TIMER0_TAILR_R = 3*16000000;

    /*Clear Timer0 timeout flag*/
    TIMER0_ICR_R = 0x00000001;

    /*Enable Timer0 after the configuration*/
    TIMER0_CTL_R |= (0x01);
}

void PORTF_init(void)
{
    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    /*Polling until the clock is started and GPIO Port F is ready for access*/
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){}

    /*Allow changes to PF1:PF3 pins*/
    GPIO_PORTF_CR_R |= 0x0E;

    /*Declare PF1:PF3 as output pins*/
    GPIO_PORTF_DIR_R = 0x0E;

    /*Disable alternate functions on all pins of PORT F*/
    GPIO_PORTF_AFSEL_R = 0x00;

    /*Enable the digital functions of PF1:PF3*/
    GPIO_PORTF_DEN_R = 0x0E;
}

int main ()
{
    /*Counter for changing the lighting color in order*/
    static uint8_t counter=0;

    /*Initialize PF1:PF3 for LEDs*/
    PORTF_init();

    /*Initialize Timer0 for 32-bit, periodic, down-counter, 3-seconds period*/
    periodicTimer0Init();

    while (1)
    {
        /*Check if the Timer0 period has ended by checking Timer0 flag*/
        if (TIMER0_RIS_R & 0x01)
        {
            switch (counter)
            {
            case 0:
                /*Turn on the Red LED*/
                GPIO_PORTF_DATA_R = RED;
                break;
            case 1:
                /*Turn on the Green LED*/
                GPIO_PORTF_DATA_R = GREEN;
                break;
            case 2:
                /*Turn on the Blue LED*/
                GPIO_PORTF_DATA_R = BLUE;
                break;
            default :
                /*Turn off all LEDs*/
                GPIO_PORTF_DATA_R = 0;
                break;
            }
            /*Clear the timer flag to be able to catch the next timer-flag raising*/
            TIMER0_ICR_R |= 0x01;

            /*Increment the counter to change the lighting color to the next one*/
            counter++;

            /*When all the 3 LEDs lighted and switched off, start lighting again from the first LED*/
            if (counter==4)
            {
                counter=0;
            }
        }
    }
}

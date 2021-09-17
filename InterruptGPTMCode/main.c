/*
 * Toggle Red LED every 3 seconds using a general purpose timer interrupt
 *
 *  Created on: 12/09/2021
 *      Author: Ahmed El Daly
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

void SystemInit (){}

void EnableInterrupts(void);
void WaitForInterrupt(void);

void Timer0_Task (void)
{
    /*Toggle the red LED*/
    GPIO_PORTF_DATA_R ^= 0x02;
}

void Timer0A_Handler (void)
{
    /*Clear Timer0 timeout flag*/
    TIMER0_ICR_R = 0x00000001;

    /*Handler call back function*/
    (*Timer0_Task)();
}


void PORTF_init(void)
{
    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    /*Polling until the clock is started and GPIO Port F is ready for access*/
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)){}

    /*Unlock the GPIO PORT F*/
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;

    /*Allow changes to PF0:PF4 pins*/
    GPIO_PORTF_CR_R |= 0x02;

    /*Declare PF1 as output pin*/
    GPIO_PORTF_DIR_R=0x02;

    /*Disable alt function for PF1*/
    GPIO_PORTF_AFSEL_R &= ~0x02;

    /*Enable digital function for PF1*/
    GPIO_PORTF_DEN_R=0x02;
}

void Timer0A_Init(void)
{
    /*Activate Timer0*/
    SYSCTL_RCGCTIMER_R |= 0x01;

    /*Disable Timer0 before configuration*/
    TIMER0_CTL_R = 0x00000000;

    /*Timer0A mode: 32-bit mode*/
    TIMER0_CFG_R = 0x00000000;

    /*Timer0A mode: periodic , count down mode*/
    TIMER0_TAMR_R = 0x00000002;

    /*Start counting from 3 seconds with 16 MHz frequency*/
    TIMER0_TAILR_R = 3*16000000;

    /*Clear Timer0A timeout flag*/
    TIMER0_ICR_R = 0x00000001;

    /*Enable timeout interrupt*/
    TIMER0_IMR_R = 0x00000001;

    /*Enable Timer0A NVIC*/
    NVIC_EN0_R = 1<<19;

    /*Enable Timer0 After configuration*/
    TIMER0_CTL_R = 0x00000001;
}

int main (void)
{
    /*Initialize PF1 for LED*/
    PORTF_init();

    /*Initialize Timer0 for 32-bit, periodic, down-counter, 3-seconds period*/
    Timer0A_Init();

    EnableInterrupts();

    while (1)
    {
        /*Power saving wait for interrupt mode*/
        WaitForInterrupt();
    }
}

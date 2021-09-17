/*
 * FreeRTOS LED control through UART0
 *
 *  Created on: 15/09/2021
 *      Author: Ahmed El Daly
 */
#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils/uartstdio.h"

#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

void SystemInit(void){}
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  UARTStringSend
 * --------------------
 *  Description: a function that receives the UART base address and string to send it through UART Tx port
 *
 *  Parameters:
 *  	ui32Base: The required UART base address
 *  	Str     : The string required to be sent
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/

static void UARTStringSend(uint32_t ui32Base,uint8_t *Str)
{
    /*Counter definition for looping over the to be sent string*/
    uint8_t i=0;

    for (i=0; Str[i] != '\0'; i++)
    {
        /*Send the string character by character*/
        UARTCharPut(ui32Base,Str[i]);
    }
}
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  UARTStringGet
 * --------------------
 *  Description: a function that receives the UART base address and string to receive it through UART Rx port
 *
 *  Parameters:
 *  	ui32Base: The required UART base address
 *  	Str     : The string required to be received
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/

static void UARTStringGet(uint32_t ui32Base,uint8_t *Str)
{
    /*Counter definition for looping over the to be received string*/
    uint8_t i = 0;

    /*Get the first character through UART Rx*/
    Str[i] = UARTCharGet(ui32Base);

    /*Receive characters until receiving the 'Enter' character*/
    while(Str[i] != 13)
    {
        /*Receive the next character in the next position of the string*/
        i++;
        Str[i] = UARTCharGet(ui32Base);
    }

    /*Enter the terminating character at the string end*/
    Str[i] = '\0';
}
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
static void vTask1 (void * p)
{
    /*String to receive the frame*/
    uint8_t frame [7];

    /*Acknowledge the user with the required communication frame*/
    UARTStringSend(UART0_BASE,"\n\rWrite \"RxGxBx\" then press Enter\n\r"
            "where 'x' takes values : 0 or 1 for turning the corresponding LED off or on respectively\n\r\n\r");

    while (1)
    {
        /*Receive the frame string*/
        UARTStringGet(UART0_BASE,frame);

        /*If the frame is valid turn on LEDs through the corresponding values in the frame*/
        if ( (frame[0]=='R') && (frame[2]=='G') && (frame[4]=='B') )
        {
            if ( ( (frame[1]-'0') <=1 ) && ( (frame[3]-'0') <=1 ) && ( (frame[5]-'0') <=1 ) )
            {
                /*Turn off or on the Red LED on PF1 based on the corresponding value in the frame*/
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1  ,(frame[1]<<1));

                /*Turn off or on the Green LED on PF1 based on the corresponding value in the frame*/
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3  ,(frame[3]<<3));

                /*Turn off or on the Blue LED on PF1 based on the corresponding value in the frame*/
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2  ,(frame[5]<<2));


                UARTStringSend(UART0_BASE,"LEDs Turned on Successfully\n\r");
            }
            else
            {
                /*Acknowledge the user that the frame is invalid and LEDs state will not change*/
                UARTStringSend(UART0_BASE,"Invalid Frame\n\r");
            }
        }
        else
        {
            /*Acknowledge the user that the frame is invalid and LEDs state will not change*/
            UARTStringSend(UART0_BASE,"Invalid Frame\n\r");
        }
    }
}
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
static void PORTF_Init(void)
{
    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    /*Unlock the GPIO PF1:PF3*/
    GPIOUnlockPin(GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /*Declare PF1:PF3 as output pins*/
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

static void UART_Init (void)
{
    /*Enable and provide a clock to UART0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){ }

    /*Enable and provide a clock to GPIO Port A in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}

    /*Configure Pin0 for UART0 Rx and Pin1 for UART1 Tx*/
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /*Enable UART0 with baudrate 115200, 8-bits data, one stop bit, no parity bit*/
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200 ,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}
/*---------------------------------------------------------------------------------------------------*/

int main(void)
{
    /*Enable clock with divider of 1, use OSC, main oscillator source, 16MHz external crystal frequency*/
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /*Initialize PF1:PF3 for LEDs*/
    PORTF_Init();

    /*Initialize PA0,PA1 for UART Rx, Tx respectively*/
    UART_Init();

    /*Create Task1 with stack size of 1000, and priority: 1*/
    xTaskCreate(vTask1, "Task1", 1000,NULL,1,NULL);

    vTaskStartScheduler();

    while(1) {}

    return 0;
}

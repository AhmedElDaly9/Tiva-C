#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "utils/uartstdio.h"

#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#define RED 0x02
#define GREEN 0x08
#define BLUE 0x04

TaskHandle_t vTask1Handle=NULL;
TaskHandle_t vTask2Handle=NULL;
TaskHandle_t vTask3Handle=NULL;


uint8_t RedCh = 'R';
uint8_t BlueCh = 'B';
uint8_t GreenCh = 'G';

void vTask( void *pvParameters )
{
    volatile uint32_t counter;

    for (;;)
    {
        for (counter=0 ; counter<2000000 ; counter++)
        {
            //Just making the task busy
        }
        switch (*((uint8_t *)pvParameters))
        {
        case 'R':
        case 'r':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, RED);
            vTaskPrioritySet(vTask1Handle, 1);
            vTaskPrioritySet(vTask2Handle, 2);
            vTaskPrioritySet(vTask3Handle, 1);
            break;
        case 'G':
        case 'g':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GREEN);
            vTaskPrioritySet(vTask1Handle, 1);
            vTaskPrioritySet(vTask2Handle, 1);
            vTaskPrioritySet(vTask3Handle, 2);
            break;
        case 'B':
        case 'b':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, BLUE);
            vTaskPrioritySet(vTask1Handle, 2);
            vTaskPrioritySet(vTask2Handle, 1);
            vTaskPrioritySet(vTask3Handle, 1);
            break;
        default:
            break;
        }
    }
}

static void UART_Init (void)
{
    /*Enable and provide a clock to UART0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){ }

    /*Enable and provide a clock to GPIO Port A in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}

    /*Enable UART0 with baudrate 115200, 8-bits data, one stop bit, no parity bit*/
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200 ,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    /*Configure Pin0 for UART0 Rx and Pin1 for UART1 Tx*/
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}
static void PORTF_Init(void)
{
    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    /*Unlock the GPIO PF1:PF3*/
    GPIOUnlockPin(GPIO_PORTF_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    /*Declare PF1:PF3 as output pins*/
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /*Declare PF0 as input pin*/
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);

    /*Configure PF0 for a pull-up resistor*/
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
/*---------------------------------------------------------------------------------------------------*/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    PORTF_Init();
    UART_Init();

    xTaskCreate(vTask, "Task1", 1000, (void *) &RedCh, 1, &vTask1Handle);
    xTaskCreate(vTask, "Task2", 1000, (void *) &GreenCh, 1, &vTask2Handle);
    xTaskCreate(vTask, "Task3", 1000, (void *) &BlueCh, 1, &vTask3Handle);

    vTaskStartScheduler();

    for (;;);

    return 0;
}

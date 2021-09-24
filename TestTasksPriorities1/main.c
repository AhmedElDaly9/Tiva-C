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

TaskHandle_t vTask1Handle=NULL;
TaskHandle_t vTask2Handle=NULL;
/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  UARTStringSend
 * --------------------
 *  Description: a function that receives the UART base address and string to send it through UART Tx port
 *
 *  Parameters:
 *      ui32Base: The required UART base address
 *      Str     : The string required to be sent
 *
 *  returns:
 *      (void)
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

void vTask1( void *pvParameters )
{
    UBaseType_t Task1Priority = uxTaskPriorityGet(NULL);

    for (;;)
    {
        UARTStringSend(UART0_BASE ,"Task 1 is running\r\n");
        vTaskPrioritySet(vTask2Handle, Task1Priority+1);
    }
}

void vTask2( void *pvParameters )
{
    UBaseType_t Task2Priority = uxTaskPriorityGet(NULL);

    for( ;; )
    {
        UARTStringSend(UART0_BASE ,"Task 2 is running\r\n" );
        vTaskPrioritySet(vTask2Handle, Task2Priority-2);
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
/*---------------------------------------------------------------------------------------------------*/

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    UART_Init();

    xTaskCreate(vTask1, "Task1", 1000, (void *) "Task1\n\r", 2, &vTask1Handle);
    xTaskCreate(vTask2, "Task2", 1000, (void *) "Task2\n\r", 1, &vTask2Handle);


    vTaskStartScheduler();

    for (;;);

    return 0;
}

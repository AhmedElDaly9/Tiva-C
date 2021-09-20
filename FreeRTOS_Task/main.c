/*
 * FreeRTOS LED control through UART0
 * The UART receives the inputs 'R' or 'G' or 'B' from the user corresponding to the required to toggle LED,
 * Then the sender task sends the valid inputs to a queue,
 * The auto-reload timer gives the semaphore required to make the signal of receiving a character in queue,
 * When the receiver task is able to take the semaphore it toggles the required LED,
 * SW2 is able to change the auto-reload timer periods to be either 3sec or 5sec or 10sec.
 *
 *
 *  Created on: 15/09/2021
 *      Author: Ahmed El Daly
 */
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "utils/uartstdio.h"

#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

void EnableInterrupts(void);
void SystemInit(void){}

#define Qsize 3

static uint8_t g_UART_Data;

static QueueHandle_t g_QueueHandle = NULL;

static SemaphoreHandle_t xSemphUARTReceive = NULL;
static SemaphoreHandle_t xSemphQueueReceive = NULL;

static TimerHandle_t xTimerReceive = NULL;
BaseType_t xTimerReceiveStarted;

TickType_t TimerReceivePeriods[3] = {3000,5000,10000};


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



/*---------------------------------------------------------------------------------------------------*/
void RXTimerCallBack (TimerHandle_t xTimerReceive)
{
    /*The Receiver task can receive the next input queue after a certain period of time*/
    xSemaphoreGiveFromISR(xSemphQueueReceive,NULL);
}

void UART0_INT_Handler ()
{
    /*Receive the input character*/
    g_UART_Data = UARTCharGet(UART0_BASE);

    /*Acknowledge the SenderTask that a character is received*/
    xSemaphoreGiveFromISR(xSemphUARTReceive,NULL);

    /*Clear the flag of UART receive timeout interrupt*/
    UARTIntClear(UART0_BASE,UART_INT_RT);
}
/*---------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------*/
void SenderTask (void * p)
{
    /*Local variable to receive the global UART input variable for the task manipulation*/
    uint8_t LEDcolor;

    while (1)
    {
        if (xSemaphoreTake(xSemphUARTReceive,portMAX_DELAY)==pdTRUE)
        {
            LEDcolor = g_UART_Data;

            /*Send the input to queue if and only if the input is valid*/
            switch (LEDcolor)
            {
            case 'R':
            case 'r':
                if (xQueueSend(g_QueueHandle, (void *) (&LEDcolor) ,(TickType_t)0)== pdTRUE)
                {
                    UARTStringSend(UART0_BASE, "--> Red LED Toggling In Queue\n\r");

                    /*Timer starts with every input to make sure that the last input will be served for the same period*/
                    xTimerStart(xTimerReceive,(TickType_t)0);
                }
                else
                {
                    UARTStringSend(UART0_BASE, "!Not Enough Space In Queue\n\r");
                }
                break;
            case 'G':
            case 'g':
                if (xQueueSend(g_QueueHandle, (void *) (&LEDcolor) ,(TickType_t)0)== pdTRUE)
                {
                    UARTStringSend(UART0_BASE, "--> Green LED Toggling In Queue\n\r");
                    xTimerStart(xTimerReceive,(TickType_t)0);
                }
                else
                {
                    UARTStringSend(UART0_BASE, "!Not Enough Space In Queue\n\r");
                }
                break;
            case 'B':
            case 'b':
                if (xQueueSend(g_QueueHandle, (void *) (&LEDcolor) ,(TickType_t)0)== pdTRUE)
                {
                    UARTStringSend(UART0_BASE, "--> Blue LED Toggling In Queue\n\r");
                    xTimerStart(xTimerReceive,(TickType_t)0);
                }
                else
                {
                    UARTStringSend(UART0_BASE, "!Not Enough Space In Queue\n\r");
                }
                break;
            default:
                UARTStringSend(UART0_BASE, "!Invalid input\n\r");
                break;
            }
        }
    }
}

void ReceiverTask (void * p)
{
    /*Local variable to receive the queues inputs for the task manipulation*/
    uint8_t LEDcolor;

    /*Flag for the state of empty queue to avoid multiple prints*/
    uint8_t flag=0;

    while (1)
    {
        if (xSemaphoreTake(xSemphQueueReceive,portMAX_DELAY)==pdTRUE)
        {
            if (xQueueReceive(g_QueueHandle, (void *) (&LEDcolor) ,(TickType_t)0) == pdTRUE)
            {
                switch (LEDcolor)
                {
                case 'R':
                case 'r':
                    /**/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1 ,~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));
                    UARTStringSend(UART0_BASE,"$$ Red LED Toggled Successfully\n\r");
                    flag=0;
                    break;
                case 'G':
                case 'g':
                    /**/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3 ,~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3));
                    UARTStringSend(UART0_BASE,"$$ Green LED Toggled Successfully\n\r");
                    flag=0;
                    break;
                case 'B':
                case 'b':
                    /**/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2 ,~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2));
                    UARTStringSend(UART0_BASE,"$$ Blue LED Toggled Successfully\n\r");
                    flag=0;
                    break;
                default:
                    break;
                }
            }
            else
            {
                /*If the queue is clear, turn off all LEDs*/
                if (flag==0)
                {
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1| GPIO_PIN_2 |GPIO_PIN_3,0);
                    UARTStringSend(UART0_BASE,"$$ No inputs left in Queue, LEDs are Turned off\n\r");
                    flag=1;
                }
            }

        }
    }
}

void GPIO_TimerPeriodTask (void *p)
{
    /*Local variable to be TimerReceivePeriods array index*/
    uint8_t index=0;

    while (1)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0)
        {
            /*300ms delay to avoid bouncing effect*/
            vTaskDelay(pdMS_TO_TICKS(300));

            if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0)
            {
                xQueueReset(g_QueueHandle);

                index++;

                if (index == 3)
                {
                    index=0;
                }

                switch (index)
                {
                case 0:
                    xTimerChangePeriod(xTimerReceive,pdMS_TO_TICKS(TimerReceivePeriods[index]),(TickType_t)0);
                    UARTStringSend (UART0_BASE,"Timer period reset to 3 seconds, Queue is cleared\n\r");
                    break;

                case 1:
                    xTimerChangePeriod(xTimerReceive,pdMS_TO_TICKS(TimerReceivePeriods[index]),(TickType_t)0);
                    UARTStringSend (UART0_BASE,"Timer period increased to 5 seconds, Queue is cleared\n\r");
                    break;
                case 2:
                    xTimerChangePeriod(xTimerReceive,pdMS_TO_TICKS(TimerReceivePeriods[index]),(TickType_t)0);
                    UARTStringSend (UART0_BASE,"Timer period increased to 10 seconds, Queue is cleared\n\r");
                    break;
                default:
                    UARTStringSend (UART0_BASE,"Invalid Timer period, ERROR\n\r");
                    break;
                }
            }
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
    GPIOUnlockPin(GPIO_PORTF_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );

    /*Declare PF1:PF3 as output pins*/
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /*Declare PF0 as input pin*/
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);

    /*Configure PF0 for a pull-up resistor*/
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
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

    /*Enable FIFO buffer*/
    UARTFIFOEnable(UART0_BASE);

    /*Enable UART receive timeout interrupt*/
    UARTIntRegister(UART0_BASE,UART0_INT_Handler);
    UARTIntEnable(UART0_BASE, UART_INT_RT);
    IntEnable(INT_UART0);

    IntMasterEnable();
}
/*---------------------------------------------------------------------------------------------------*/

int main(void)
{
    /*Setting the clock frequency to 80MHz*/
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /*Initialize PF0 as input and PF1:PF3 as outputs*/
    PORTF_Init();

    /*Initialize UART for UART0 and Enable UART Interrupts*/
    UART_Init();

    /*Auto-Reload Timer to determine the rate of receiving from queue*/
    xTimerReceive = xTimerCreate (
            "Receiver Timer",
            pdMS_TO_TICKS(TimerReceivePeriods[0]),
            pdTRUE,(void *) 0,
            RXTimerCallBack);

    /*Create characters queue for receiving inputs from UART*/
    g_QueueHandle = xQueueCreate (Qsize, sizeof(uint8_t));
    assert(g_QueueHandle != NULL);

    /*Binary semaphore to make a signal of receiving a character through UART*/
    xSemphUARTReceive = xSemaphoreCreateBinary();
    assert(xSemphUARTReceive != NULL);

    /*Binary semaphore to make a signal of sending a character in queue*/
    xSemphQueueReceive = xSemaphoreCreateBinary();
    assert(xSemphQueueReceive != NULL);

    /*Create the required tasks with the same priority to handle them using time-slicing technique*/
    xTaskCreate(ReceiverTask, "ReceiverTask", 1000,NULL,1,NULL);
    xTaskCreate(SenderTask, "SenderTask", 1000,NULL,1,NULL);
    xTaskCreate(GPIO_TimerPeriodTask, "Change Timer Period Task", 1000, NULL, 1, NULL);

    /*Making sure that all the tasks are created successfully*/
    if (ReceiverTask != NULL && SenderTask != NULL && GPIO_TimerPeriodTask != NULL)
    {
        UARTStringSend(UART0_BASE,"Enter 'R' for Toggling Red LED "
                       "or 'G' for Toggling Green LED or 'B' for Toggling Blue LED\n\r");

        vTaskStartScheduler();
    }

    while(1) {}

    return 0;
}

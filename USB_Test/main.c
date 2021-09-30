/*
 * Turn on LEDs from data received from USB
 *
 *  Created on: 25/09/2021
 *      Author: Ahmed El Daly
 */
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "usb_handlers.h"

void USB_serialInit();

/*Global flag to indicate that the usb buffer has data to be read*/
volatile uint32_t g_ui32Flag = 0;

/*Global flag indicating that a USB configuration has been set*/
volatile bool g_bUSBConfigured = false;

#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08

/*
 Function Name: USB_serialInit

 This function is called by to initialize the usb driver as a Communication
 Device Class "CDC"
 */
void USB_serialInit()
{

    /*Configure the required pins for USB operation.*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);

    /*Initialize the transmit and receive buffers.*/
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);

    /*Set the USB stack mode to Device mode with VBUS monitoring.*/
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    /*Pass our device information to the USB library and place the device
    on the bus.*/
    USBDCDCInit(0, &g_sCDCDevice);

}

int main(void)
{
    uint8_t dataReceivedFromPC;

    /*Setting the clock frequency to 80MHz*/
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);

    /*Initalize USB with buffer size 256 and baudrate 115200*/
    USB_serialInit();

    /*Enable and provide a clock to GPIO Port F in Run mode*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    /*Declare PF1:PF3 as output pins*/
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    while(1)
    {
        if(g_ui32Flag == 1)
        {
            USBBufferRead((tUSBBuffer *)&g_sRxBuffer, &dataReceivedFromPC, 1);

            switch(dataReceivedFromPC)
            {
            case 'r':
            case 'R':
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,RED);
                break;
            case 'g':
            case 'G':
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GREEN);
                break;
            case 'b':
            case 'B':
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,BLUE);
                break;
            default:
                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x00);
                break;
            }
            g_ui32Flag=0;
        }
    }


}

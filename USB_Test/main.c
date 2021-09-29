/*****************************************************************************
    This is an example that uses USB CDC to communicate with the pc using a
    serial terminal to receive a byte and echo it to the terminal again.
    If the pc sends r,g,b the led color will be red or green or blue. sending any
    other character will turn the led off
 *****************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "usb_handlers.h"   /*Header file for usb handlers that has everthing*/

#define Numbers_Ascii_Base 0x30

uint8_t letter_index = 0;
void USB_serialInit();

/*A flag to indicate that the usb buffer has data to read it.*/
volatile uint32_t g_ui32Flag = 0;

/*Global flag indicating that a USB configuration has been set*/
volatile bool g_bUSBConfigured = false;


static float f_ASCIItoDecimal (uint8_t * Data)
{
    uint8_t negative_flag = 0;
    uint8_t point_index = 0;
    uint8_t multiplier = 10;
    float number = 0.0;

    /* Loop on the given array to determine the the decimal point location, letter location, and check if the number is negative */
    while (Data[letter_index] != '\0')
    {
        switch (Data[letter_index])
        {
        case '.':
            point_index = letter_index++;
            break;

        case '-':
            negative_flag = 1;
            letter_index++;
            break;

        default:
            letter_index++;
            break;
        }
    }

    /* deceremt the letter_index by 1 because it was pointing at the '\0' */
    letter_index --;

    /* If the data was a whole number with no decimal points, we can iterate to the end of the numbers*/
    if (point_index == 0)
    {
        uint8_t i;
        for (i = negative_flag ; i<letter_index ; i++)
        {
            number *= 10;
            Data[i] = Data[i] -  Numbers_Ascii_Base;
            number = number + Data[i];
        }
    }
    /* If the data had decimals, we must process the number on two steps. First the whole numbers, then the decimals */
    else
    {
        uint8_t i;
        for (i = negative_flag ; i<point_index ; i++)
        {
            number *= 10;
            Data[i] = Data[i] -  Numbers_Ascii_Base;
            number = number + Data[i];
        }

        i +=1;

        while (i<letter_index)
        {
            Data[i] = Data[i] - Numbers_Ascii_Base;
            number = number + (float)(Data[i]) / (float)(multiplier);
            multiplier *= 10;
            i++;
        }
    }

    /* If the number was negative, we must multiply with -1 */
    if (negative_flag == 1)
        number = number * -1;

    return number;
}



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
void USB_receiveString(uint8 *Str)
{
    uint8 i = 0;
    Str[i] = USBBufferRead((tUSBBuffer *)&g_sRxBuffer, Str[i], 1);  /*Reading the value sent from pc*/
    ;
    while(Str[i] != 13)
    {
        i++;
        Str[i] = USBBufferRead((tUSBBuffer *)&g_sRxBuffer, Str[i], 1);
    }
    Str[i] = '\0';
}
int main(void)
{
    uint8_t dataReceivedFromPC[10];
    uint8_t numReceivedFromPC[10];
    uint8_t counter=0;
    uint8_t flag=1;
    uint8_t LEDx;

    /*80 MHz Clock*/
    SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    USB_serialInit();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    /*Enabling PORTF*/
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);    /*Enabling internal red  LEDs*/

    while(1)
    {
        if(g_ui32Flag == 1)
        {
            USB_receiveString(dataReceivedFromPC);
            //USBBufferRead((tUSBBuffer *)&g_sRxBuffer, dataReceivedFromPC, 6);  /*Reading the value sent from pc*/
            //USBBufferWrite((tUSBBuffer *)&g_sTxBuffer,dataReceivedFromPC, 5);  /*Sending the value again to print it on the terminal*/
            //dataReceivedFromPC[5]='\0';

            while (flag && counter<10)
            {
                switch (dataReceivedFromPC[counter])
                {
                case 'r':
                case 'R':
                case 'g':
                case 'G':
                case 'b':
                case 'B':
                    flag=0;
                    break;
                default :
                    numReceivedFromPC[counter] = dataReceivedFromPC[counter];
                    counter++;
                }
            }
            numReceivedFromPC[counter]='\0';

            LEDx = f_ASCIItoDecimal (numReceivedFromPC);

            if (LEDx <=100.0 && LEDx >= -100.0)
            {
                switch(dataReceivedFromPC[counter])
                {
                case 'r':
                case 'R':/*LED will be turned on with red color*/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                                 (GPIO_PIN_1)|(!GPIO_PIN_2)|(!GPIO_PIN_3));
                    break;
                case 'g':
                case 'G':/*LED will be turned on with green color*/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                                 (!GPIO_PIN_1)|(!GPIO_PIN_2)|(GPIO_PIN_3));
                    break;
                case 'b':
                case 'B':/*LED will be turned on with blue color*/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                                 (!GPIO_PIN_1)|(GPIO_PIN_2)|(!GPIO_PIN_3));
                    break;
                default:        /*LED will be turned off*/
                    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                                 (!GPIO_PIN_1)|(!GPIO_PIN_2)|(!GPIO_PIN_3));
                    break;
                }
                USBBufferFlush(&g_sTxBuffer);   /* Flush our buffers.*/
                USBBufferFlush(&g_sRxBuffer);
                g_ui32Flag=0;   /*clearing the flag */
            }
        }


    }
}

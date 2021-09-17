#include <stdint.h>
#include "GPIO.h"

/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  portNum
 * --------------------
 *  Description: a function that receives the port address and returns the port number
 *
 *  Parameters:
 *		port: The required port address
 *
 *  returns: 
 *		uint8 : The required port number
 *
 *---------------------------------------------------------------------------------------------------*/
uint8 portNum (uint32 port)
{
	switch (port)
	{
		case PORTA:
			return PORTA_NUM;
		
		case PORTB:
			return PORTB_NUM;
		
		case PORTC:
			return PORTC_NUM;
		
		case PORTD:
			return PORTD_NUM;
		
		case PORTE:
			return PORTE_NUM;
		
		case PORTF:
			return PORTF_NUM;
	}
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  enableClk
 * --------------------
 *  Description: a function that enables and provide clocks for ports
 *
 *  Parameters:
 *		port: The required port address
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void enableClk (uint32 port)
{
	uint8 portNumber=portNum(port);
	
	SET_BIT(SYS_CTL,RCGCGPIO,portNumber);
	while (BIT_IS_CLEAR(SYS_CTL,RCGCGPIO,portNumber)){}
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  unlockPort
 * --------------------
 *  Description: a function that unlocks the required to manipulate port
 *
 *  Parameters:
 *		port: The required port address
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void unlockPort (uint32 port,uint8 pin)
{
	REG_VAL(port,GPIOLOCK)= GPIO_UNLOCK_KEY;
	SET_BIT(port,GPIOCR,pin);
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  lockPort
 * --------------------
 *  Description: a function that locks the required to lock port
 *
 *  Parameters:
 *		port: The required port address
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void lockPort (uint32 port)
{
	REG_VAL(port,GPIOLOCK)= GPIO_LOCK;
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  pinDir
 * --------------------
 *  Description: a function that determines the direction of pins Input or Output
 *
 *  Parameters:
 *		port: The required port address
 *		pin	: The required to manipulate pin number
 *		dir : the pin is required to be INPUT or OUTPUT pin
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void pinDir (uint32 port,uint8 pin,uint8 dir)
{
	if (dir == OUTPUT)
	{
		SET_BIT(port,GPIODIR,pin);
	}
	else if (dir == INPUT)
	{
		CLEAR_BIT(port,GPIODIR,pin);
	}
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  pullRes
 * --------------------
 *  Description: a function that determines the pull type of the pin
 *
 *  Parameters:
 *		port: The required port address
 *		pin	: The required to manipulate pin number
 *		pull: the pull is required to be OPEN_DRAIN or PULL_DOWN or PULL_UP
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void pullRes(uint32 port,uint8 pin,uint8 pull)
{
	switch (pull)
	{
		case OPEN_DRAIN:
			SET_BIT(port,GPIOODR,pin);
			break;
				
		case PULL_UP:
			SET_BIT(port,GPIOPUR,pin);
			break;
		
		case PULL_DOWN:
			SET_BIT(port,GPIOPDR,pin);
			break;

		default:
			break;
	}
	return;
}	


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  digitalEn
 * --------------------
 *  Description: a function that enable digital functions of pins
 *
 *  Parameters:
 *		port: The required port address
 *		pin	: The required to manipulate pin number
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void digitalEn (uint32 port,uint8 pin)
{
	SET_BIT(port,GPIODEN,pin);
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  disableAlt
 * --------------------
 *  Description: a function that disable alternate functions of pins
 *
 *  Parameters:
 *		port: The required port address
 *		pin	: The required to manipulate pin number
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
static void disableAlt (uint32 port,uint8 pin)
{
	CLEAR_BIT(port,GPIOAFSEL,pin);
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  readBit
 * --------------------
 *  Description: a function that reads the value of a particular bit in a register
 *
 *  Parameters:
 *		port: The required port or register address
 *		bit	: The required to read bit number
 *
 *  returns: 
 *		uint: The read bit value
 *
 *---------------------------------------------------------------------------------------------------*/
uint8 readBit (uint32 port,uint32 bit)
{
	return (BIT_IS_SET(port,GPIODATA,bit));
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  readBit
 * --------------------
 *  Description: a function that reads the value of a particular bit in a register
 *
 *  Parameters:
 *		port: The required port or register address
 *		bit	: The required to write bit number
 *		data: The value required to be written
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
void writeBit (uint32 port,uint32 bit,uint8 data)
{
	if (data == HIGH)
	{
		SET_BIT(port,GPIODATA,bit);
	}
	else if (data == LOW)
	{
		CLEAR_BIT(port,GPIODATA,bit);
	}
	return;
}


/*---------------------------------------------------------------------------------------------------*/
/*
 * Function:  digitalGPIOPinConfig
 * --------------------
 *  Description: a function that does all the required intialization for digital pins
 *
 *  Parameters:
 *		config: A structure contains the required settings for the digital pin (port, pin, dir, pull)
 *
 *  returns: 
 *		(void)
 *
 *---------------------------------------------------------------------------------------------------*/
void digitalGPIOPinConfig (PinConfig* config)
{
	/*Enable and provide a clock to the required GPIO port in Run mode*/
	enableClk(config->port);

	/*Unlock the GPIO required port*/
	unlockPort(config->port,config->pin);
	
	/*Determine the port direction*/
	pinDir(config->port,config->pin,config->dir);
	
	/*Determine the pull type*/
	pullRes(config->port,config->pin,config->pull);
	
	/*Enable digital functions for the required pin*/
	digitalEn(config->port,config->pin);
	
	/*Disable analog functions for the required pin*/
	disableAlt(config->port,config->pin);
	
	return;
}


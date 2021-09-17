#ifndef GPIO_H
#define GPIO_H

/************************************************************************
*						Includes
************************************************************************/
#include "common_macros.h"
#include "std_types.h"
#include "GPIO_REG_ADDRESSES.h"

/************************************************************************
*						Enums and Structures
************************************************************************/
typedef enum
{
	OPEN_DRAIN,PULL_UP,PULL_DOWN
}ResistorPull;

typedef struct
{
	uint32 port;
	uint8 pin;
	uint8 dir;
	uint8 pull;
}PinConfig;

/************************************************************************
*						Functions prototypes
************************************************************************/
uint8 portNum (uint32 port);
static void enableClk (uint32 port);
static void unlockPort (uint32 port,uint8 pin);
static void lockPort (uint32 port);
static void pinDir (uint32 port,uint8 pin,uint8 dir);
static void pullRes(uint32 port,uint8 pin,uint8 pull);
static void digitalEn (uint32 port,uint8 pin);
static void disableAlt (uint32 port,uint8 pin);
uint8 readBit (uint32 port,uint32 bit);
void writeBit (uint32 port,uint32 bit,uint8 data);
void digitalGPIOPinConfig (PinConfig* config);

#endif

	
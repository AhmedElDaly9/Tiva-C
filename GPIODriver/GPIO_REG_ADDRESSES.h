#ifndef GPIO_REG_ADDRESSES
#define GPIO_REG_ADDRESSES

#define GPIODATA 			0x3FC
#define GPIODIR 			0x400
#define GPIOIS 				0x404
#define GPIOIBE 			0x408
#define GPIOIEV 			0x40C
#define GPIOIM 				0x410
#define GPIORIS 			0x414
#define GPIOMIS 			0x418
#define GPIOICR 			0x41C
#define GPIOAFSEL 		0x420
#define GPIODR2R 			0x500
#define GPIODR4R 			0x504
#define GPIODR8R 			0x508
#define GPIOODR 			0x50C
#define GPIOPUR 			0x510
#define GPIOPDR 			0x514
#define GPIOSLR 			0x518
#define GPIODEN 			0x51C
#define GPIOLOCK 			0x520
#define GPIOCR 				0x524
#define GPIOAMSEL 		0x528
#define GPIOPCTL			0x52C
#define GPIOADCCTL 		0x530

#define RCGCGPIO 			0x608

#define SYS_CTL 							0x400FE000

#define GPIO_UNLOCK_KEY       0x4C4F434B
#define GPIO_LOCK           	0xFFFFFFFF

#endif
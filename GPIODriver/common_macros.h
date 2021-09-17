 /******************************************************************************
 *
 * Module: Common macros
 *
 * File Name: common_macros.h
 *
 * Description: Commonly used Macros for TM4C123GH6PM
 *
 * Author: Ahmed El Daly
 *
 *******************************************************************************/

#ifndef COMMON_MACROS
#define COMMON_MACROS

#define PORTA 0x40004000
#define PORTB 0x40005000
#define PORTC 0x40006000
#define PORTD 0x40007000
#define PORTE 0x40024000
#define PORTF 0x40025000

#define PORTA_NUM 0
#define PORTB_NUM 1
#define PORTC_NUM 2
#define PORTD_NUM 3
#define PORTE_NUM 4
#define PORTF_NUM 5

#define PIN0 0
#define PIN1 1
#define PIN2 2
#define PIN3 3
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7 

#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7
#define BIT8 8
#define BIT9 9
#define BIT10 10
#define BIT11 11
#define BIT12 12
#define BIT13 13
#define BIT14 14
#define BIT15 15
#define BIT16 16
#define BIT17 17
#define BIT18 18
#define BIT19 19
#define BIT20 20
#define BIT21 21
#define BIT22 22
#define BIT23 23
#define BIT24 24
#define BIT25 25
#define BIT26 26
#define BIT27 27
#define BIT28 28
#define BIT29 29
#define BIT30 30
#define BIT31 31

#define REG_VAL(PORT,REG) (*((volatile uint32 *)((PORT)+(REG))))

/* Set a certain bit in any register */
#define SET_BIT(PORT,REG,BIT) (REG_VAL(PORT,REG)|=(1<<BIT))

/* Clear a certain bit in any register */
#define CLEAR_BIT(PORT,REG,BIT) (REG_VAL(PORT,REG)&=(~(1<<BIT)))

/* Toggle a certain bit in any register */
#define TOGGLE_BIT(PORT,REG,BIT) (REG_VAL(PORT,REG)^=(1<<BIT))

/* Rotate right the register value with specific number of rotates */
#define ROR(PORT,REG,num) ( REG_VAL(PORT,REG)= (REG_VAL(PORT,REG)>>num) | (REG_VAL(PORT,REG)<<(8-num)) )

/* Rotate left the register value with specific number of rotates */
#define ROL(PORT,REG,num) ( REG_VAL(PORT,REG)= (REG_VAL(PORT,REG)<<num) | (REG_VAL(PORT,REG)>>(8-num)) )

/* Check if a specific bit is set in any register and return true if yes */
#define BIT_IS_SET(PORT,REG,BIT) ( REG_VAL(PORT,REG) & (1<<BIT) )

/* Check if a specific bit is cleared in any register and return true if yes */
#define BIT_IS_CLEAR(PORT,REG,BIT) ( !(REG_VAL(PORT,REG) & (1<<BIT)) )

#endif



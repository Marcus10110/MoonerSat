/*
 * SatDefines.h
 *
 *  Created on: Dec 21, 2014
 *      Author: Mark
 */

#ifndef SATDEFINES_H_
#define SATDEFINES_H_

#include <msp430.h>

/*
UCB0 used for SPI radio modules

UCB0SOMI	1.7
UCB0SIMO 	1.6
UCB0CLK		2.2

CS pins:
RF0 CS 		1.2
RF1 CS 		1.3

Ant ctrl lines:
RF0 TXANT	1.4
RF0	RXANT	1.5
RF1 TXANT	3.0
RF1 RXANT	3.4

 *
 */


#define msleep(x)   __delay_cycles( x * 8000 )
#define usleep(x)   __delay_cycles( x * 8 )

#define RF0_CS_PIN		BIT2
#define	RF1_CS_PIN		BIT3
#define RF0_TXANT_PIN	BIT4
#define RF0_RXANT_PIN	BIT5
#define RF1_TXANT_PIN	BIT0
#define RF1_RXANT_PIN	BIT4

#define RF0_CS_PORT		P1OUT
#define RF1_CS_PORT		P1OUT
#define RF0_TXANT_PORT	P1OUT
#define RF0_RXANT_PORT	P1OUT
#define RF1_TXANT_PORT	P3OUT
#define RF1_RXANT_PORT	P3OUT

#define MISO_PIN		BIT7
#define MOSI_PIN		BIT6
#define CLK_PIN			BIT2


//CS active low
#define RF0_CS_ON	RF0_CS_PORT &= ~RF0_CS_PIN
#define RF0_CS_OFF		RF0_CS_PORT |= RF0_CS_PIN

#define RF1_CS_ON	RF1_CS_PORT &= ~RF1_CS_PIN
#define RF1_CS_OFF		RF1_CS_PORT |= RF1_CS_PIN

//ant ctrl active high
#define RF0_RXANT_ON	RF0_RXANT_PORT |= RF0_RXANT_PIN
#define RF0_RXANT_OFF	RF0_RXANT_PORT &= ~RF0_RXANT_PIN

#define RF1_RXANT_ON	RF1_RXANT_PORT |= RF1_RXANT_PIN
#define RF1_RXANT_OFF	RF1_RXANT_PORT &= ~RF1_RXANT_PIN

#define RF0_TXANT_ON	RF0_TXANT_PORT |= RF0_TXANT_PIN
#define RF0_TXANT_OFF	RF0_TXANT_PORT &= ~RF0_TXANT_PIN

#define RF1_TXANT_ON	RF1_TXANT_PORT |= RF1_TXANT_PIN
#define RF1_TXANT_OFF	RF1_TXANT_PORT &= ~RF1_TXANT_PIN




#endif /* SATDEFINES_H_ */

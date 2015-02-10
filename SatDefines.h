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

#define P1_BASE_ADD		((void*)(P1_BASE))
#define P2_BASE_ADD		((void*)(P1_BASE+1))
#define P3_BASE_ADD		((void*)(P3_BASE))
#define P4_BASE_ADD		((void*)(P3_BASE+1))


#define msleep(x)   __delay_cycles( x * 8 )
#define usleep(x)   __delay_cycles( x * 8 )

#define RF0_CS_PIN		BIT2
#define	RF1_CS_PIN		BIT3
#define RF0_TXANT_PIN	BIT4
#define RF0_RXANT_PIN	BIT5
#define RF1_TXANT_PIN	BIT0
#define RF1_RXANT_PIN	BIT4

#define RF0_CS_PORT		P1_BASE_ADD
#define RF1_CS_PORT		P1_BASE_ADD
#define RF0_TXANT_PORT	P1_BASE_ADD
#define RF0_RXANT_PORT	P1_BASE_ADD
#define RF1_TXANT_PORT	P3_BASE_ADD
#define RF1_RXANT_PORT	P3_BASE_ADD

#define UCB0_MISO_PIN		BIT7	//1.7
#define UCB0_MOSI_PIN		BIT6	//1.6
#define UCB0_CLK_PIN		BIT2	//2.2

#define UCA1_MISO_PIN		BIT6	//2.6
#define UCA1_MOSI_PIN		BIT5	//2.5
#define UCA1_CLK_PIN		BIT4	//2.4

#define UCA0_MISO_PIN		BIT1	//2.1
#define UCA0_MOSI_PIN		BIT0	//2.0
#define UCA0_CLK_PIN		BIT5	//1.5

#define SDA_PIN				BIT6	//1.6
#define CSL_PIN				BIT7	//1.7

#define UART_TX_PIN			BIT0	//2.0
#define UART_RX_PIN			BIT1	//2.1
//CS active low

//ant ctrl active high





#endif /* SATDEFINES_H_ */

/*
 * SatPeripherals.c
 *
 *  Created on: Dec 20, 2014
 *      Author: Mark
 */



#include "SatPeripherals.h"
#include "SatDefines.h"

void InitIo()
{
    //special function control registers
    //bank 1: 2,3,4,5
    //bank 3: 0,4
    P1SEL0 &= ~(RF0_CS_PIN | RF1_CS_PIN | RF0_TXANT_PIN | RF0_RXANT_PIN);
    P1SEL1 &= ~(RF0_CS_PIN | RF1_CS_PIN | RF0_TXANT_PIN | RF0_RXANT_PIN);
    P3SEL0 &= ~( RF1_TXANT_PIN | RF1_RXANT_PIN );
    P3SEL1 &= ~( RF1_TXANT_PIN | RF1_RXANT_PIN );

    //UCB0 SPI: 1.6, 1.7, 2.2
    P1SEL0 &= ~( MISO_PIN | MOSI_PIN );
    P1SEL1 |= MISO_PIN | MOSI_PIN;
    P2SEL0 &= ~CLK_PIN;
    P2SEL1 |= CLK_PIN;


    //direction control
    P1DIR |= RF0_CS_PIN | RF1_CS_PIN | RF0_TXANT_PIN | RF0_RXANT_PIN;
    P3DIR |= RF1_TXANT_PIN | RF1_RXANT_PIN;

    //unlock pin configuration after exit from LPM5. definately required, but I've never seen this before.
    PM5CTL0 &= ~LOCKLPM5;

    //inital state
    RF0_CS_OFF;
    RF1_CS_OFF;
    RF0_RXANT_OFF;
    RF1_RXANT_OFF;
    RF0_TXANT_OFF;
    RF1_TXANT_OFF;
}

void InitSpi()
{
	/*
	RFM22B SPI details:
	Max speed 10 MHz
	MSB first
	MSB of address is R/W bit. Set means Write.
	sel is active low
	clock is idle low
	RFM reads data on rising & leading edge. (we should set up on the neg edge)
	RFM sets up data on the negative edge. (we should read it on the pos edge)
	*/

	//UCB 0 in 3 wire SPI mode, master.
	UCB0CTLW0 = UCSWRST;		//hold module in reset

	//clock polarity: ~UCCKPL
	//msb first: UCMSB
	//phase: UCCKPH
	//master mode: UCMST
	//UCMODE_0 3 pin spi
	//UCSYNC
	//UCSSEL__SMCLK

	UCB0CTLW0 |= UCMSB | UCCKPH | UCMST | UCSYNC | UCSSEL__SMCLK;

	//baud rate, SMclock / 8. randomly chosen.
	UCB0BRW = 0x0080;

	UCB0CTLW0 &= ~UCSWRST;

	UCB0IE = 0; //disable all interrupts.
}
U8 ReadWriteSpi( U8 tx_data )
{
	//this function does not prep the EN lines.
	U8 rx_data;

	//wait for the TX interrupt to be set, indicating we can write again.
	while( (UCB0IFG & UCTXIFG) == 0 );
	UCB0TXBUF = tx_data;

	//wait for the RX interrupt to be set, indicating that data was read.
	while( (UCB0IFG & UCRXIFG) == 0 );
	rx_data = UCB0RXBUF;

	//wait for the clock line to finish it's last cycle and go low.
	while( ( UCB0STATW & UCBUSY ) != 00 );

	return rx_data;
}

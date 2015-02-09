/*
 * spi.c
 *
 *  Created on: Feb 7, 2015
 *      Author: markg_000
 */

#include "spi.h"
#include <msp430.h>
#include "SatDefines.h"


spi_err_t spi_setup()
{
	//setup IO for SPI ports.
	//UCB0 SPI: 1.6, 1.7, 2.2
	P1SEL0 &= ~( MISO_PIN | MOSI_PIN );
	P1SEL1 |= MISO_PIN | MOSI_PIN;
	P2SEL0 &= ~CLK_PIN;
	P2SEL1 |= CLK_PIN;



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

	return SPI_OK;
}

spi_err_t spi_init( spi_t *spi, gpio_t *cs_pin )
{
	spi->CsPin = cs_pin;

	gpio_set( cs_pin, GPIO_HIGH );

	return SPI_OK;
}

spi_err_t spi_read_write_buffer( spi_t *spi, U8 *tx_buffer, U8 *rx_buffer, U8 count )
{
	//this function does not prep the EN lines.
	U8 rx_data;
	U8 tx_data;
	U8 i;

	gpio_set( spi->CsPin, GPIO_LOW );

	__delay_cycles( 80 );

	for( i = 0; i < count; ++i )
	{
		if( tx_buffer != NULL )
		{
			tx_data = tx_buffer[i];
		}
		else
		{
			tx_data = 0x00;
		}

		//wait for the TX interrupt to be set, indicating we can write again.
		while( (UCB0IFG & UCTXIFG) == 0 );
		UCB0TXBUF = tx_data;

		//wait for the RX interrupt to be set, indicating that data was read.
		while( (UCB0IFG & UCRXIFG) == 0 );
		rx_data = UCB0RXBUF;

		//wait for the clock line to finish it's last cycle and go low.
		while( ( UCB0STATW & UCBUSY ) != 00 );


		if( rx_buffer != NULL )
		{
			rx_buffer[i] = rx_data;
		}


	}

	gpio_set( spi->CsPin, GPIO_HIGH );

	__delay_cycles( 80 );

	return SPI_OK;
}

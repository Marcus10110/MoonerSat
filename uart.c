/*
 * uart.c
 *
 *  Created on: Feb 8, 2015
 *      Author: markg_000
 */


#include "uart.h"
#include "SatDefines.h"
#include <msp430.h>


uart_err_t uart_setup()
{
	return UART_OK;
}

uart_err_t uart_init( uart_t *uart )
{
	P2SEL0 &= ~( UART_TX_PIN | UART_RX_PIN );
	P2SEL1 |= UART_TX_PIN | UART_RX_PIN;


	//A0 for now, this is the 'backchannel' port.
	UCA0CTLW0 = UCSWRST;
	//LSB first, no parity, 8 bit, one stop bit, UART, async, SMCLK, IE off.
	UCA0CTLW0 |= UCSSEL__SMCLK;

	UCA0BRW = 833;

	UCA0CTLW0 &= ~UCSWRST;

	return UART_OK;
}

uart_err_t uart_set_baud( uart_t *uart, U16 baud )
{
	U32 temp = 8000000;

	U16 count = temp / baud;

	//UCA0CTLW0 |= UCSWRST;

	//baud rate calculation.
	//UCA0BRW = count;

	//UCA0CTLW0 &= ~UCSWRST;

	return UART_OK;
}

uart_err_t uart_write_byte( uart_t *uart, U8 value )
{
	//wait for buffer to be empty
	while( ( UCA0IFG & UCTXIFG ) == 0 );


	UCA0TXBUF = value;

	return UART_OK;
}

uart_err_t uart_is_byte_availible( uart_t *uart, bool *is_availible )
{
	if( (UCA0IFG & UCRXIFG) == 0 )
		*is_availible = FALSE;
	else
		*is_availible = TRUE;

	return UART_OK;
}

uart_err_t uart_read_byte( uart_t *uart, U8* buffer )
{

	*buffer = UCA0RXBUF;

	return UART_OK;
}

/*
 * i2c.c
 *
 *  Created on: Feb 9, 2015
 *      Author: markg_000
 */

#include "i2c.h"
#include <msp430.h>

i2c_err_t i2c_setup()
{
#define SDA_PIN				BIT6	//1.6
#define CSL_PIN				BIT7	//1.7

	P1SEL0 &= ~( SDA_PIN | CSL_PIN );
	P1SEL1 |= SDA_PIN | CSL_PIN;

	UCB0CTLW0 = UCSWRST;		//hold module in reset

	UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL__SMCLK;

	//baud rate, 8M -> 100K.
	UCB0BRW = 0x0050;

	UCB0CTLW0 &= ~UCSWRST;

	UCB0IE = 0; //disable all interrupts.
	return I2C_OK;
}
i2c_err_t i2c_init( i2c_t *i2c )
{
	return I2C_OK;
}

i2c_err_t i2c_write( i2c_t *i2c, U8 address, U8* data, U8 count )
{
	return I2C_OK;
}
i2c_err_t i2c_read( i2c_t *i2c, U8 address, U8* buffer, U8 count )
{
	return I2C_OK;
}


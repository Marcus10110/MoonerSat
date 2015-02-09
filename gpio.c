/*
 * gpio.c
 *
 *  Created on: Feb 7, 2015
 *      Author: markg_000
 */

#include "gpio.h"
#include <msp430.h>
//this is the MSP430 specific GPIO impl. It's packing sensistive.
typedef struct {
	U8 In;				//00
	U8 pad_0;
	U8 Out;				//02
	U8 pad_1;
	U8 Direction;		//04
	U8 pad_2;
	U8 ResistorEnable;	//06
	U8 pad_3[3];
	U8 Select0;			//0A
	U8 pad_4;
	U8 Select1;			//0C
	U8 pad_5;
} msp430_gpio_t;

gpio_err_t gpio_setup()
{
	//unlock pin configuration after exit from LPM5. definately required, but I've never seen this before.
	PM5CTL0 &= ~LOCKLPM5;
	return GPIO_OK;
}



gpio_err_t gpio_init( gpio_t *gpio )
{
	gpio_state_t temp;

	//validate port base address for our chip. (from data sheet)
	//base addresses:
	//port1: 0x0200	P1_BASE
	//port2: 0x0201	P1_BASE+1
	//port3: 0x0220	P3_BASE
	//port4: 0x0221	P3_BASE+1
	int base_int = (int)(gpio->Port);
	volatile msp430_gpio_t* port = (msp430_gpio_t*)gpio->Port;

	switch( base_int )
	{
	case P1_BASE: //intentional fall through
	case P1_BASE+1:
	case P3_BASE:
	case P3_BASE+1:
		break;
	default:
		return GPIO_ERR_INVALID;
	}

	//clear the sel bits.
	(port->Select0) &= ~(gpio->Pin);
	(port->Select1) &= ~(gpio->Pin);

	//set to input:

	gpio_get( gpio, &temp );
	return GPIO_OK;

}

gpio_err_t gpio_set( gpio_t *gpio, gpio_state_t state )
{

	volatile msp430_gpio_t* port = (msp430_gpio_t*)gpio->Port;

	//set output bit.
	port->Direction |= gpio->Pin;

	//set state.
	if( state == GPIO_HIGH )
	{
		port->Out |= (U8)gpio->Pin;
	}
	else if( state == GPIO_LOW )
	{
		port->Out &= ~((U8)gpio->Pin);
	}
	else
	{
		return GPIO_ERR_INVALID;
	}

	return GPIO_OK;

}

gpio_err_t gpio_get( gpio_t *gpio, gpio_state_t *state )
{
	volatile msp430_gpio_t* port = (msp430_gpio_t*)gpio->Port;

	//clear output bit.
	port->Direction &= ~(gpio->Pin);

	if( (port->In & gpio->Pin) == 0x00 )
		*state = GPIO_LOW;
	else
		*state = GPIO_HIGH;


	return GPIO_OK;
}

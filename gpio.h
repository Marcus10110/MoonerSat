/*
 * gpio.h
 *
 *  Created on: Feb 7, 2015
 *      Author: markg_000
 */

#ifndef GPIO_H_
#define GPIO_H_


#include "SatTypes.h"


typedef struct {
	void* Port;
	U16 Pin;
} gpio_t;

typedef enum {
	GPIO_ERR_INVALID,
	GPIO_OK
} gpio_err_t;

typedef enum {
	GPIO_INPUT,
	GPIO_OUTPUT
} gpio_direction_t;

typedef enum {
	GPIO_LOW,
	GPIO_HIGH
} gpio_state_t;

gpio_err_t gpio_setup();
gpio_err_t gpio_init( gpio_t *gpio );
gpio_err_t gpio_set( gpio_t *gpio, gpio_state_t state );
gpio_err_t gpio_get( gpio_t *gpio, gpio_state_t *state );




#endif /* GPIO_H_ */

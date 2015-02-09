/*
 * uart.h
 *
 *  Created on: Feb 8, 2015
 *      Author: markg_000
 */

#ifndef UART_H_
#define UART_H_

#include "SatTypes.h"

typedef struct {


} uart_t;

typedef enum {
	UART_ERR_INVALID,
	UART_OK

} uart_err_t;

uart_err_t uart_setup();
uart_err_t uart_init( uart_t *uart );

uart_err_t uart_set_baud( uart_t *uart, U16 baud );

uart_err_t uart_write_byte( uart_t *uart, U8 value );
uart_err_t uart_is_byte_availible( uart_t *uart, bool *is_availible );
uart_err_t uart_read_byte( uart_t *uart, U8* buffer );


#endif /* UART_H_ */

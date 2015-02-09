/*
 * spi.h
 *
 *  Created on: Feb 7, 2015
 *      Author: markg_000
 */

#ifndef SPI_H_
#define SPI_H_

#include "SatTypes.h"
#include "gpio.h"

typedef struct {
	gpio_t *CsPin;
} spi_t;


typedef enum {
	SPI_ERR_INVALID,
	SPI_OK
} spi_err_t;

spi_err_t spi_setup();

spi_err_t spi_init( spi_t *spi, gpio_t *cs_pin );

spi_err_t spi_read_write_buffer( spi_t *spi, U8 *tx_buffer, U8 *rx_buffer, U8 count );






#endif /* SPI_H_ */

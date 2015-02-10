/*
 * i2c.h
 *
 *  Created on: Feb 9, 2015
 *      Author: markg_000
 */

#ifndef I2C_H_
#define I2C_H_

#include "SatTypes.h"

typedef struct {



} i2c_t;

typedef enum {
	I2C_ERR_INVALID,
	I2C_ERR_NAK,
	I2C_OK
} i2c_err_t;


i2c_err_t i2c_setup();
i2c_err_t i2c_init( i2c_t *i2c );

i2c_err_t i2c_write( i2c_t *i2c, U8 address, U8* data, U8 count );
i2c_err_t i2c_read( i2c_t *i2c, U8 address, U8* buffer, U8 count );

#endif /* I2C_H_ */

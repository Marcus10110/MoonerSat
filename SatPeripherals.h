/*
 * SatPeripherals.h
 *
 *  Created on: Dec 20, 2014
 *      Author: Mark
 */

#ifndef SATPERIPHERALS_H_
#define SATPERIPHERALS_H_

#include "SatTypes.h"

void InitIo();
void InitSpi();
U8 ReadWriteSpi( U8 tx_data );



#endif /* SATPERIPHERALS_H_ */

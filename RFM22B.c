/*
 * RFM22B.c
 *
 *  Created on: Dec 20, 2014
 *      Author: Mark
 */

#include "RFM22B.h"
#include "SatDefines.h"
#include "gpio.h"
#include "spi.h"

typedef struct {
	U32 baud_bps;
	U8 dwn3_bypass;
	U8 ndec_exp;
	U8 filset;
	U16 rxosr;
	U32 ncoff;
	U16 crgain;
} ModemRxSettings;


const ModemRxSettings ModemSettings[] = {
		{ 2000, 0x0, 0x3, 0x3, 0x0FA, 0x8312, 0x06B }, // BAUD_2K_FDEV_5K
		{ 2400, 0x0, 0x3, 0x3, 0x0D0, 0x09D49, 0x0A0 }, // BAUD_2_4K_FDEV_4_8K
		{ 2400, 0x0, 0x0, 0x1, 0x683, 0x013A9, 0x5 }, // BAUD_2_4K_FDEV_36K
		{ 4800, 0x0, 0x3, 0x4, 0x68, 0x13A93, 0x278 }, // BAUD_4_8K_FDEV_4_8K
		{ 4800, 0x0, 0x0, 0x4, 0x341, 0x2752, 0x00A }, // BAUD_4_8K_FDEV_45K
		{ 9600, 0x0, 0x2, 0x1, 0x68, 0x13A93, 0x4EE }, // BAUD_9_6K_FDEV_4_8K
		{ 9600, 0x0, 0x0, 0x4, 0x1A1, 0x04EA5, 0x24 }, // BAUD_9_6K_FDEV_45K
		{ 10000, 0x0, 0x2, 0x1, 0x64, 0x147AE, 0x521 }, // BAUD_10K_FDEV_5K
		{ 10000, 0x0, 0x0, 0x3, 0x190, 0x051EC, 0x02B }, // BAUD_10K_FDEV_40K
		{ 19200, 0x0, 0x1, 0x1, 0x68, 0x13A93, 0x4EE }, // BAUD_19_2K_FDEV_9_6K
		{ 20000, 0x0, 0x1, 0x1, 0x64, 0x147AE, 0x521 }, // BAUD_20K_FDEV_10K
		{ 20000, 0x0, 0x0, 0x4, 0x0C8, 0x0A3D7, 0x0A6 }, // BAUD_20K_FDEV_40K
		{ 38400, 0x0, 0x0, 0x1, 0x68, 0x13A93, 0x4D5 }, // BAUD_38_4K_FDEV_19_6K
		{ 40000, 0x0, 0x0, 0x1, 0x64, 0x147AE, 0x521 }, // BAUD_40K_FDEV_20K
		{ 40000, 0x0, 0x0, 0x5, 0x64, 0x147AE, 0x291 }, // BAUD_40K_FDEV_40K
		{ 50000, 0x0, 0x0, 0x1, 0x50, 0x1999A, 0x668 }, // BAUD_50K_FDEV_25K
		{ 57600, 0x0, 0x0, 0x3, 0x45, 0x1D7DC, 0x76E }, // BAUD_57_6K_FDEV_28_8K
		{ 100000, 0x1, 0x0, 0xF, 0x78, 0x11111, 0x446 }, // BAUD_100K_FDEV_50K
		{ 100000, 0x1, 0x0, 0xE, 0x78, 0x11111, 0x0B8 }, // BAUD_100K_FDEV_300K
		{ 125000, 0x1, 0x0, 0x8, 0x60, 0x15555, 0x2AD }, // BAUD_125K_FDEV_125K
};


U16 compute_baud_registers( U32 dataRate_bps );
rfm_err_t set_baud_rate( rfm22b_t *rfm, U32 dataRate_bps );
rfm_err_t rfm_write_register( rfm22b_t *rfm, U8 address, U8 data );
rfm_err_t rfm_read_register( rfm22b_t* rfm, U8 address, U8 *buffer );
rfm_err_t rfm_set_antenna( rfm22b_t *rfm, rfm_antenna_state_t state );



rfm_err_t rfm_setup()
{
	return RFM_OK;
}

rfm_err_t rfm_init( rfm22b_t *rfm, spi_t *spi, gpio_t *tx_pin, gpio_t *rx_pin )
{
	rfm->Spi = spi;
	rfm->TxAntPin = tx_pin;
	rfm->RxAntPin = rx_pin;

	//disable all interrupts
	rfm_write_register( rfm, RFREG_INT_EN_2, 0x00 );

	//enter ready mode (xtal on, PLL off)
	rfm_write_register( rfm, RFREG_FUNC_CTRL_1, xton ); //xton

	//set crystal oscillator load capacitance to 0x7F. (no idea why)
	rfm_write_register( rfm, RFREG_LOAD_CAP, 0x7F ); //xlc[6:0]


	//enable TX and RX packet handling, and CRC generation. MSB first.
	rfm_write_register( rfm, RFREG_DATA_ACCESS_CTRL, enpacrx | crc_0_ccitt | encrc | enpactx );

	//set all header/packet info
	//Header control: broadcast address check is enabled for all 4 bytes.
	//all 4 received header bytes are checked against the check header bytes too.
	rfm_write_register( rfm, RFREG_HEADER_CTRL_1, 0xFF );

	//prealen[8] = 0
	//synclen[1:0] = 1, sync word 3 and 2.
	//fixpklen = 0, packet length is indicated in header
	//hdlen[2:0] = 4, header 3, 2, 1, 0 used
	rfm_write_register( rfm, RFREG_HEADER_CTRL_2, 0x42 );

	//prealen[7:0] = 0x40, 64 dec. thats 32 bytes of preamble, or 256 bits.
	rfm_write_register( rfm, RFREG_PREAMBLE_LEN, 0x40 );

	//preath[4:0] = 0x4, 4 nibbles are processed during detection. (16 bits)
	rfm_write_register( rfm, RFREG_PREAMBLE_DETECT_CTRL, 0x20 );

	//sync word, sync[31:0]
	rfm_write_register( rfm, RFREG_SYNC_WORD_3, 0x2D );
	rfm_write_register( rfm, RFREG_SYNC_WORD_2, 0xD4 );
	rfm_write_register( rfm, RFREG_SYNC_WORD_1, 0x00 );
	rfm_write_register( rfm, RFREG_SYNC_WORD_0, 0x00 );

	//transmit header, txhd[31:0]
	rfm_write_register( rfm, RFREG_TX_HEADER_3, 's' );
	rfm_write_register( rfm, RFREG_TX_HEADER_2, 'o' );
	rfm_write_register( rfm, RFREG_TX_HEADER_1, 'n' );
	rfm_write_register( rfm, RFREG_TX_HEADER_0, 'g' );

	//pklen[7:0] = 17 dec. affects transmitted packets, not RX since packets include length.
	rfm_write_register( rfm, RFREG_TX_PACKET_LEN, 17 );

	//Check header, chhd[31:0]
	rfm_write_register( rfm, RFREG_CHK_HEADER_3, 's' );
	rfm_write_register( rfm, RFREG_CHK_HEADER_2, 'o' );
	rfm_write_register( rfm, RFREG_CHK_HEADER_1, 'n' );
	rfm_write_register( rfm, RFREG_CHK_HEADER_0, 'g' );

	//Header enable, hden[31:0]. mask used to select which bits are compared between incoming packet and our check header
	rfm_write_register( rfm, RFREG_HEADER_EN_3, 0xFF );
	rfm_write_register( rfm, RFREG_HEADER_EN_2, 0xFF );
	rfm_write_register( rfm, RFREG_HEADER_EN_1, 0xFF );
	rfm_write_register( rfm, RFREG_HEADER_EN_0, 0xFF );



	//modulation
	//turn on FIFO mode, GFSK
	rfm_write_register( rfm, RFREG_MOD_MODE_CTRL_2, modtyp_3_gfsk | dtmod_2_fifo );
	//RFREG_MOD_MODE_CTRL_1 set in data rate area


	//IF Bandwidth filter
	rfm_write_register( rfm, RFREG_MODEM_TEST, 0x01 );

	rfm_write_register( rfm, RFREG_FREQ_DEV, 0x48 );

	rfm_write_register( rfm, RFREG_VCO_CURRENT_TRIM, 0x7F );

	rfm_write_register( rfm, RFREG_DRIVIDER_CURRENT_TRIM, 0x40 );

	rfm_write_register( rfm, RFREG_CHARGEPUMP_CURRENT_OVERRIDE, 0x80 );

	rfm_write_register( rfm, RFREG_AGC_OVERRIDE_2, 0x0B );

	rfm_write_register( rfm, RFREG_DELTA_ADC_TUNE_2, 0x04 );

	return RFM_OK;
}

rfm_err_t rfm_set_baud( rfm22b_t *rfm, rfm_modem_speed_t data_rate )
{
	//settings from chart for 9600 baud, 45 fdev
	U8 dwn3_bypass = 0x00;
	U8 ndec_exp = 0x00;
	U8 filset = 0x04;
	U16 rxosr = 0x01A1;
	U32 ncoff = 0x04EA5;
	U16 crgain = 0x0024;
	U32 baud_rate = 9600;

	//data rate registers.
	U8	txdr_lower = 0x00;
	U8	txdr_upper = 0x00;
	U16 txdr = 0x0000;

	const ModemRxSettings* modem_settings = &ModemSettings[data_rate];



	dwn3_bypass = modem_settings->dwn3_bypass;
	ndec_exp = modem_settings->ndec_exp;
	filset = modem_settings->filset;
	rxosr = modem_settings->rxosr;
	ncoff = modem_settings->ncoff;
	crgain = modem_settings->crgain;
	baud_rate = modem_settings->baud_bps;


	//set baud rate
	if ( baud_rate < 30000 ) { // Use txdtrtscale = 1.
		rfm_write_register( rfm, RFREG_MOD_MODE_CTRL_1, txdtrtscale );
	} else {
		rfm_write_register( rfm, RFREG_MOD_MODE_CTRL_1, 0x00 );
	}

	txdr = compute_baud_registers( baud_rate );
	txdr_lower = (U8)txdr;
	txdr_upper = (U8)( txdr >> 8 );
	//data rate below 30 kbps. manchester off. data whitening off.
	rfm_write_register( rfm, RFREG_TX_DATA_RATE_1, txdr_upper ); //txdr[15:8]
	rfm_write_register( rfm, RFREG_TX_DATA_RATE_0, txdr_lower ); //txdr[7:0]

	//100kbps/300 fdev
	/*dwn3_bypass = 0x01;
	ndec_exp = 0x00;
	filset = 0x0E;
	rxosr = 0x0078;
	ncoff = 0x11111;
	crgain = 0x02AD;*/

	U8 reg = 0;
	//RFREG_IF_BANDWIDTH (1C)
	reg = filset;
	reg |= ndec_exp << 4;
	reg |= dwn3_bypass << 7;
	rfm_write_register( rfm, RFREG_IF_BANDWIDTH, reg );

	//RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO (20)
	reg = rxosr;
	rfm_write_register( rfm, RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO, reg );

	//RFREG_CLK_REC_OFFSET_2(21)
	reg = ncoff >> 16;
	reg |= ((rxosr & 0x0700) >> 8) << 5;
	rfm_write_register( rfm, RFREG_CLK_REC_OFFSET_2, reg );

	//RFREG_CLK_REC_OFFSET_1 (22)
	reg = ncoff >> 8;
	rfm_write_register( rfm, RFREG_CLK_REC_OFFSET_1, reg );

	//RFREG_CLK_REC_OFFSET_0 (23)
	reg = ncoff;
	rfm_write_register( rfm, RFREG_CLK_REC_OFFSET_0, reg );

	//RFREG_CLK_REC_TIMING_LOOP_GAIN_1 (24)
	reg = crgain >> 8;
	rfm_write_register( rfm, RFREG_CLK_REC_TIMING_LOOP_GAIN_1, reg );

	//RFREG_CLK_REC_TIMING_LOOP_GAIN_0 (25)
	reg = crgain;
	rfm_write_register( rfm, RFREG_CLK_REC_TIMING_LOOP_GAIN_0, reg );

	rfm_write_register( rfm, RFREG_AFC_GEARSHIFT_OVERRIDE, 0x40 );
	rfm_write_register( rfm, RFREG_CLK_RECOVERY_GEARSHIFT_OVERRIDE, 0x03 );

	return RFM_OK;
}

rfm_err_t rfm_set_frequency( rfm22b_t *rfm, U32 frequency_hz )
{
	//this function should have greater than 1khz resolution.
	U8 hbsel_val = 0;
	U8 fb = 0;
	U16 fc = 0;
	U32 temp = 0;
	U8 reg = 0;

	if( frequency_hz < 240000000 )
		return RFM_ERR_INVALID;

	if( frequency_hz > 930000000 )
		return RFM_ERR_INVALID;


	//check if high band should be enabled.
	if( frequency_hz >= 480000000 )
		hbsel_val = 1;

	//find fband, integer component.
	fb = ((frequency_hz-(240000000 * (hbsel_val + 1)))/10000000);

	if( hbsel == 1 )
		fb = ((frequency_hz-480000000)/10000000);


	//fc = ( frequency_hz / ( 10000000 * (hbsel + 1) ) - fb - 24 ) * 64000;
	temp = ( 24ul + fb ) * 10000000ul * ( hbsel_val + 1 );
	temp = frequency_hz - temp;
	temp = temp * 64;
	temp = temp / (10000 * (hbsel_val + 1));
	fc = temp;

	reg = fb | ( hbsel_val << 5 ) | sbsel;
	rfm_write_register( rfm, RFREG_FREQ_BAND_SEL, sbsel | reg );
	rfm_write_register( rfm, RFREG_NOMINAL_CARRIER_FREQ_1, fc >> 8 ); //fc[15:8]
	rfm_write_register( rfm, RFREG_NOMINAL_CARRIER_FREQ_0, fc ); //fc[7:0]

	return RFM_OK;
}

rfm_err_t rfm_set_power( rfm22b_t *rfm, U8 tx_power_dbm )
{
	//tx power
	rfm_write_register( rfm, RFREG_TX_POWER, 0x00 ); //there could be an error in the datasheet. This should be max power.
	return RFM_OK;
}

rfm_err_t rfm_tx_data( rfm22b_t *rfm, U8* data, U8 count )
{
	U8 i;
	U8 rx;

	if( count > 17 )
		return RFM_ERR_OVERRUN;

	rfm_write_register( rfm, RFREG_FUNC_CTRL_1, xton);	// To ready mode

	rfm_set_antenna( rfm, ANT_TX );
	msleep( 5 );

	rfm_write_register( rfm, RFREG_FUNC_CTRL_2, 0x03 );	// FIFO reset
	rfm_write_register( rfm, RFREG_FUNC_CTRL_2, 0x00 );	// Clear FIFO

	rfm_write_register(rfm, RFREG_PREAMBLE_LEN, 64);	// preamble = 64nibble
	rfm_write_register(rfm, RFREG_TX_PACKET_LEN, count );	// packet length = count


	for( i = 0; i < count; ++i)
	{
		rfm_write_register(rfm, RFREG_FIFO_ACCESS, data[i]);
	}



	rfm_write_register(rfm, RFREG_INT_EN_1, 0x04);			// enable packet sent interrupt
	rfm_read_register(rfm, RFREG_INT_STATUS_1, &rx);		// Read Interrupt status1 register
	rfm_read_register(rfm, RFREG_INT_STATUS_2, &rx);

	rfm_write_register(rfm, RFREG_FUNC_CTRL_1, txon | xton );	// Start TX

	//while ((PIND & (1<<NIRQ)) != 0); 	// need to check interrupt here
	//wait for Packet Sent interrupt to be set.
	//wait up to 500ms, 10 ms at a time.
	for( i=0; i < 50; ++i )
	{
		U8 status = 0x00;

		//msleep( 10 ); //10ms
		__delay_cycles( 80000 );

		rfm_read_register( rfm, RFREG_INT_STATUS_1, &status );
		if( ( status & (1 << 2) ) != 0 )
			break;

	}

	//__delay_cycles( 100000 * 8 );

	rfm_write_register(rfm, RFREG_FUNC_CTRL_1, xton );	// to ready mode

	rfm_set_antenna( rfm, ANT_OFF );

	return RFM_OK;
}

rfm_err_t rfm_is_data_availible( rfm22b_t *rfm, bool *has_data )
{
	//check register for interrupt. if a valid packet is not RXed, return false.
	U8 status = 0x00;
	rfm_read_register( rfm, RFREG_INT_STATUS_2, &status );
	status = 0x00;
	rfm_read_register( rfm, RFREG_INT_STATUS_1, &status );
	if( ( status & (1 << 1) ) == 0 )
	{
		*has_data = FALSE;
	}
	else
	{
		*has_data = TRUE;
	}


	return RFM_OK;
}

rfm_err_t rfm_rx_data( rfm22b_t *rfm, U8* data, U8 buffer_max_length )
{
	//check register for interrupt. if a valid packet is not RXed, return false.
	U8 i;


	//woot! we got a packet!
	for( i = 0; i < buffer_max_length; ++i )
		rfm_read_register( rfm, RFREG_FIFO_ACCESS, &data[i] );

	return RFM_OK;
}

rfm_err_t rfm_start_rx( rfm22b_t *rfm )
{
	U8 status;

	rfm_set_antenna( rfm, ANT_RX );
	msleep( 50 );

	rfm_write_register( rfm, RFREG_FUNC_CTRL_1, xton );	// to ready mode

	//clear interrupts.
	rfm_read_register( rfm, RFREG_INT_STATUS_1, &status);
	rfm_read_register( rfm, RFREG_INT_STATUS_2, &status);

	rfm_write_register( rfm, RFREG_RX_FIFO_CTRL, 17);//rx fifo almost full threshold set to 17.

	rfm_write_register( rfm, RFREG_FUNC_CTRL_2, 0x03);//clear both fifos
	rfm_write_register( rfm, RFREG_FUNC_CTRL_2, 0x00);//second write in above opperation

	rfm_write_register( rfm, RFREG_INT_EN_1, 2);//enable interrupt for valid packet received

	rfm_write_register( rfm, RFREG_FUNC_CTRL_1, rxon | xton );//RX on in manual receiver mode.  auto clears when a valid packet is received. (does nothing in multiple packet config)
	//ready mode


	return RFM_OK;
}

U16 compute_baud_registers( U32 dataRate_bps )
{
	U64 temp = dataRate_bps;
	U16 txdr = 0;
	if ( dataRate_bps < 30000 ) { // Use txdtrtscale = 1.
		temp <<= 21; //times 2^21
	} else {
		temp <<= 16; //times 2^16
	}
	
	//divide by 10^6
	temp = (U32)(temp / 1000000); //compiler issue?

	if( temp > 65535 )
		return 0; //error

	txdr = temp;

	return txdr;
}


rfm_err_t rfm_write_register( rfm22b_t* rfm, U8 address, U8 data )
{

	U8 tx_data[2];

	tx_data[0] = address | 0x80;
	tx_data[1] = data;

	spi_read_write_buffer(rfm->Spi, tx_data, NULL, 2);

	return RFM_OK;
}

rfm_err_t rfm_read_register( rfm22b_t* rfm, U8 address, U8 *buffer )
{
	U8 tx_data[2];
	U8 rx_data[2];

	address &= ~0x80; //just in case
	tx_data[0] = address;
	tx_data[1] = 0x00;

	rx_data[0] = 0x00;
	rx_data[1] = 0x00;


	spi_read_write_buffer(rfm->Spi, tx_data, rx_data, 2);

	buffer[0] = rx_data[1];

	return RFM_OK;
}

rfm_err_t rfm_set_antenna( rfm22b_t *rfm, rfm_antenna_state_t state )
{

	if( state == ANT_OFF )
	{
		gpio_set( rfm->TxAntPin, GPIO_LOW );
		gpio_set( rfm->RxAntPin, GPIO_LOW );
	}
	else if( state == ANT_RX )
	{

		gpio_set( rfm->TxAntPin, GPIO_LOW );
		gpio_set( rfm->RxAntPin, GPIO_HIGH );
	}
	else if( state == ANT_TX )
	{
		gpio_set( rfm->TxAntPin, GPIO_HIGH );
		gpio_set( rfm->RxAntPin, GPIO_LOW );

	}
	return RFM_OK;
}


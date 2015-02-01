/*
 * RFM22B.c
 *
 *  Created on: Dec 20, 2014
 *      Author: Mark
 */

#include "RFM22B.h"
#include "SatPeripherals.h"
#include "SatDefines.h"


void WriteRegister( U8 radio, U8 address, U8 data );
U8 ReadRegister( U8 radio, U8 address );
void SetAnt( U8 radio, U8 ant_state );

U16 ComputeTxRateReg( U32 dataRate_bps )
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

void SetDataRate( U8 radio, U32 dataRate_bps )
{
	U8	txdr_lower = 0x00;
	U8	txdr_upper = 0x00;
	U16 txdr = 0x0000;
	if ( dataRate_bps < 30000 ) { // Use txdtrtscale = 1.
		WriteRegister( radio, RFREG_MOD_MODE_CTRL_1, txdtrtscale );
	} else {
		WriteRegister( radio, RFREG_MOD_MODE_CTRL_1, 0x00 );
	}
	
	txdr = ComputeTxRateReg( dataRate_bps );
	txdr_lower = (U8)txdr;
	txdr_upper = (U8)( txdr >> 8 );
	//data rate below 30 kbps. manchester off. data whitening off.
	//WriteRegister( radio, RFREG_TX_DATA_RATE_1, 0x27 ); //txdr[15:8]
	//WriteRegister( radio, RFREG_TX_DATA_RATE_0, 0x52 ); //txdr[7:0]
	WriteRegister( radio, RFREG_TX_DATA_RATE_1, txdr_upper ); //txdr[15:8]
	WriteRegister( radio, RFREG_TX_DATA_RATE_0, txdr_lower ); //txdr[7:0]
}

void SetFrequency( U8 radio, U32 frequency_hz )
{
	//this function should have greater than 1khz resolution.
	U8 hbsel_val = 0;
	U8 fb = 0;
	U16 fc = 0;
	U32 temp = 0;
	U8 reg = 0;

	if( frequency_hz < 240000000 )
		return;

	if( frequency_hz > 930000000 )
		return;


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
	WriteRegister( radio, RFREG_FREQ_BAND_SEL, sbsel | reg );
	WriteRegister( radio, RFREG_NOMINAL_CARRIER_FREQ_1, fc >> 8 ); //fc[15:8]
	WriteRegister( radio, RFREG_NOMINAL_CARRIER_FREQ_0, fc ); //fc[7:0]



}

void InitRfm22( U8 radio )
{
	// Variables
	// Data rate 1 - 128 kbps.
	U32 data_rate_bps = 100000;
	U32 frequency_hz = 434000000;
	
	
	//disable all interrupts
	WriteRegister( radio, RFREG_INT_EN_2, 0x00 );

	//enter ready mode (xtal on, PLL off)
	WriteRegister( radio, RFREG_FUNC_CTRL_1, xton ); //xton

	//set crystal oscillator load capacitance to 0x7F. (no idea why)
	WriteRegister( radio, RFREG_LOAD_CAP, 0x7F ); //xlc[6:0]

	SetDataRate( radio, data_rate_bps );

	SetFrequency( radio, frequency_hz );

	//enable TX and RX packet handling, and CRC generation. MSB first.
	WriteRegister( radio, RFREG_DATA_ACCESS_CTRL, enpacrx | crc_0_ccitt | encrc | enpactx );


	//set all header/packet info
	//Header control: broadcast address check is enabled for all 4 bytes.
	//all 4 received header bytes are checked against the check header bytes too.
	WriteRegister( radio, RFREG_HEADER_CTRL_1, 0xFF );

	//prealen[8] = 0
	//synclen[1:0] = 1, sync word 3 and 2.
	//fixpklen = 0, packet length is indicated in header
	//hdlen[2:0] = 4, header 3, 2, 1, 0 used
	WriteRegister( radio, RFREG_HEADER_CTRL_2, 0x42 );

	//prealen[7:0] = 0x40, 64 dec. thats 32 bytes of preamble, or 256 bits.
	WriteRegister( radio, RFREG_PREAMBLE_LEN, 0x40 );

	//preath[4:0] = 0x4, 4 nibbles are processed during detection. (16 bits)
	WriteRegister( radio, RFREG_PREAMBLE_DETECT_CTRL, 0x20 );

	//sync word, sync[31:0]
	WriteRegister( radio, RFREG_SYNC_WORD_3, 0x2D );
	WriteRegister( radio, RFREG_SYNC_WORD_2, 0xD4 );
	WriteRegister( radio, RFREG_SYNC_WORD_1, 0x00 );
	WriteRegister( radio, RFREG_SYNC_WORD_0, 0x00 );

	//transmit header, txhd[31:0]
	WriteRegister( radio, RFREG_TX_HEADER_3, 's' );
	WriteRegister( radio, RFREG_TX_HEADER_2, 'o' );
	WriteRegister( radio, RFREG_TX_HEADER_1, 'n' );
	WriteRegister( radio, RFREG_TX_HEADER_0, 'g' );

	//pklen[7:0] = 17 dec. affects transmitted packets, not RX since packets include length.
	WriteRegister( radio, RFREG_TX_PACKET_LEN, 17 );

	//Check header, chhd[31:0]
	WriteRegister( radio, RFREG_CHK_HEADER_3, 's' );
	WriteRegister( radio, RFREG_CHK_HEADER_2, 'o' );
	WriteRegister( radio, RFREG_CHK_HEADER_1, 'n' );
	WriteRegister( radio, RFREG_CHK_HEADER_0, 'g' );

	//Header enable, hden[31:0]. mask used to select which bits are compared between incoming packet and our check header
	WriteRegister( radio, RFREG_HEADER_EN_3, 0xFF );
	WriteRegister( radio, RFREG_HEADER_EN_2, 0xFF );
	WriteRegister( radio, RFREG_HEADER_EN_1, 0xFF );
	WriteRegister( radio, RFREG_HEADER_EN_0, 0xFF );


	//tx power
	WriteRegister( radio, RFREG_TX_POWER, 0x00 ); //there could be an error in the datasheet. This should be max power.

	//modulation
	//turn on FIFO mode, GFSK
	WriteRegister( radio, RFREG_MOD_MODE_CTRL_2, modtyp_3_gfsk | dtmod_2_fifo );
	//RFREG_MOD_MODE_CTRL_1 set in data rate area



	ConfigureRxModemSettings(radio,0,0);

	//IF Bandwidth filter


	WriteRegister( radio, RFREG_MODEM_TEST, 0x01 );

	WriteRegister( radio, RFREG_FREQ_DEV, 0x48 );

	WriteRegister( radio, RFREG_VCO_CURRENT_TRIM, 0x7F );

	WriteRegister( radio, RFREG_DRIVIDER_CURRENT_TRIM, 0x40 );

	WriteRegister( radio, RFREG_CHARGEPUMP_CURRENT_OVERRIDE, 0x80 );

	WriteRegister( radio, RFREG_AGC_OVERRIDE_2, 0x0B );

	WriteRegister( radio, RFREG_DELTA_ADC_TUNE_2, 0x04 );



	//packet description:
	//1-512 bytes of preamble.
	//1-4 bytes of sync word
	//0-4 bytes of TX header
	//0 or 1 byte of length
	//DATA
	//0 or 2 bytes of CRC

}

void ConfigReadBackTest( U8 radio )
{
	ReadRegister( radio, RFREG_DEVICE_TYPE );
	ReadRegister( radio, RFREG_DEVICE_VERSION );
	ReadRegister( radio, RFREG_DEVICE_STATUS );
	ReadRegister( radio, RFREG_TX_HEADER_3 );
	ReadRegister( radio, RFREG_TX_HEADER_2 );
	ReadRegister( radio, RFREG_TX_HEADER_1 );
	ReadRegister( radio, RFREG_TX_HEADER_0 );
	ReadRegister( radio, RFREG_TX_DATA_RATE_0 );
	ReadRegister( radio, RFREG_TX_DATA_RATE_1 );


}

void SentTestPacket( U8 radio )
{
	U8 i;

	WriteRegister( radio, RFREG_FUNC_CTRL_1, xton);	// To ready mode

	SetAnt( radio, ANT_TX );
	__delay_cycles( 50000 * 8 );

	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x03 );	// FIFO reset
	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x00 );	// Clear FIFO

	WriteRegister(radio, RFREG_PREAMBLE_LEN, 64);	// preamble = 64nibble
	WriteRegister(radio, RFREG_TX_PACKET_LEN, 17);	// packet length = 17bytes


	WriteRegister(radio, RFREG_FIFO_ACCESS, 'h');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'e');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'l');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'l');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'o');
	WriteRegister(radio, RFREG_FIFO_ACCESS, ' ');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'w');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'o');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'r');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'l');
	WriteRegister(radio, RFREG_FIFO_ACCESS, 'd');
	WriteRegister(radio, RFREG_FIFO_ACCESS, '!');
	WriteRegister(radio, RFREG_FIFO_ACCESS, ' ');
	WriteRegister(radio, RFREG_FIFO_ACCESS, ' ');
	WriteRegister(radio, RFREG_FIFO_ACCESS, ' ');
	WriteRegister(radio, RFREG_FIFO_ACCESS, '1');
	WriteRegister(radio, RFREG_FIFO_ACCESS, '7');
	//for (i=0; i<17; i++)
	//{
		//WriteRegister(radio, RFREG_FIFO_ACCESS, 'h');	// send payload to the FIFO
	//}

	WriteRegister(radio, RFREG_INT_EN_1, 0x04);			// enable packet sent interrupt
	i = ReadRegister(radio, RFREG_INT_STATUS_1);		// Read Interrupt status1 register
	i = ReadRegister(radio, RFREG_INT_STATUS_2);

	WriteRegister(radio, RFREG_FUNC_CTRL_1, txon | xton );	// Start TX

	//while ((PIND & (1<<NIRQ)) != 0); 	// need to check interrupt here
	//wait for Packet Sent interrupt to be set.
	//wait up to 500ms, 10 ms at a time.
	for( i=0; i < 50; ++i )
	{
		U8 status;

		__delay_cycles( 80000 ); //10ms

		status = ReadRegister( radio, RFREG_INT_STATUS_1 );
		if( ( status & (1 << 2) ) != 0 )
			break;

	}

	//__delay_cycles( 100000 * 8 );

	WriteRegister(radio, RFREG_FUNC_CTRL_1, xton );	// to ready mode

	SetAnt( radio, ANT_OFF );

}

void SetupRecieveTestPacket( U8 radio )
{
	U8 i;

	SetAnt( radio, ANT_RX );
	__delay_cycles( 50000 * 8 );

	WriteRegister( radio, RFREG_FUNC_CTRL_1, xton );	// to ready mode

	//clear interrupts.
	i = ReadRegister( radio, RFREG_INT_STATUS_1);
	i = ReadRegister( radio, RFREG_INT_STATUS_2);

	WriteRegister( radio, RFREG_RX_FIFO_CTRL, 17);//rx fifo almost full threshold set to 17.

	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x03);//clear both fifos
	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x00);//second write in above opperation

	WriteRegister( radio, RFREG_INT_EN_1, 2);//enable interrupt for valid packet received

	WriteRegister( radio, RFREG_FUNC_CTRL_1, rxon | xton );//RX on in manual receiver mode.  auto clears when a valid packet is received. (does nothing in multiple packet config)
	//ready mode


}

bool TryRecieveTestPacket( U8 radio )
{
	//check register for interrupt. if a valid packet is not RXed, return false.
	U8 status;
	U8 i;
	ReadRegister( radio, RFREG_INT_STATUS_2 );
	status = ReadRegister( radio, RFREG_INT_STATUS_1 );
	if( ( status & (1 << 1) ) == 0 )
		return FALSE;

	//woot! we got a packet!
	for( i = 0; i < 17; ++i )
		ReadRegister( radio, RFREG_FIFO_ACCESS );

	return TRUE;
}



void WriteRegister( U8 radio, U8 address, U8 data )
{
	address |= 0x80;


	if( radio == RADIO0 )
		RF0_CS_ON;
	else if( radio == RADIO1 )
		RF1_CS_ON;

	__delay_cycles(80);

	ReadWriteSpi( address );

	ReadWriteSpi( data );

	__delay_cycles(80);

	if( radio == RADIO0 )
		RF0_CS_OFF;
	else if( radio == RADIO1 )
		RF1_CS_OFF;

	__delay_cycles(400);

}

U8 ReadRegister( U8 radio, U8 address )
{
	U8 rx_data;
	address &= ~0x80; //just in case

	if( radio == RADIO0 )
		RF0_CS_ON;
	else if( radio == RADIO1 )
		RF1_CS_ON;

	__delay_cycles(80);

	ReadWriteSpi( address );

	rx_data = ReadWriteSpi( 0x00 );

	__delay_cycles(80);

	if( radio == RADIO0 )
		RF0_CS_OFF;
	else if( radio == RADIO1 )
		RF1_CS_OFF;

	__delay_cycles(400);

	return rx_data;
}

void SetAnt( U8 radio, U8 ant_state )
{
	if( radio == RADIO0 )
	{
		if( ant_state == ANT_TX )
		{
			RF0_TXANT_ON;
			RF0_RXANT_OFF;
		}
		else if( ant_state == ANT_RX )
		{
			RF0_TXANT_OFF;
			RF0_RXANT_ON;
		}
		else
		{
			RF0_TXANT_OFF;
			RF0_RXANT_OFF;
		}
	}
	else if( radio == RADIO1 )
	{
		if( ant_state == ANT_TX )
		{
			RF1_TXANT_ON;
			RF1_RXANT_OFF;
		}
		else if( ant_state == ANT_RX )
		{
			RF1_TXANT_OFF;
			RF1_RXANT_ON;
		}
		else
		{
			RF1_TXANT_OFF;
			RF1_RXANT_OFF;
		}
	}
}

void ConfigureRxModemSettings( U8 radio, U32 data_rate_bps, U32 frequency_dev_hz )
{

	//settings from chart for 9600 baud, 45 fdev
	U8 dwn3_bypass = 0x00;
	U8 ndec_exp = 0x00;
	U8 filset = 0x04;
	U16 rxosr = 0x01A1;
	U32 ncoff = 0x04EA5;
	U16 crgain = 0x0024;

	//100kbps/300 fdev
	dwn3_bypass = 0x01;
	ndec_exp = 0x00;
	filset = 0x0E;
	rxosr = 0x0078;
	ncoff = 0x11111;
	crgain = 0x02AD;

	U8 reg = 0;
	//RFREG_IF_BANDWIDTH (1C)
	reg = filset;
	reg |= ndec_exp << 4;
	reg |= dwn3_bypass << 7;
	WriteRegister( radio, RFREG_IF_BANDWIDTH, reg );

	//RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO (20)
	reg = rxosr;
	WriteRegister( radio, RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO, reg );

	//RFREG_CLK_REC_OFFSET_2(21)
	reg = ncoff >> 16;
	reg |= ((rxosr & 0x0700) >> 8) << 5;
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_2, reg );

	//RFREG_CLK_REC_OFFSET_1 (22)
	reg = ncoff >> 8;
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_1, reg );

	//RFREG_CLK_REC_OFFSET_0 (23)
	reg = ncoff;
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_0, reg );

	//RFREG_CLK_REC_TIMING_LOOP_GAIN_1 (24)
	reg = crgain >> 8;
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_1, reg );

	//RFREG_CLK_REC_TIMING_LOOP_GAIN_0 (25)
	reg = crgain;
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_0, reg );

	//WriteRegister( radio, RFREG_IF_BANDWIDTH, 0x1D );
	WriteRegister( radio, RFREG_AFC_GEARSHIFT_OVERRIDE, 0x40 );
	WriteRegister( radio, RFREG_CLK_RECOVERY_GEARSHIFT_OVERRIDE, 0x03 );
	//WriteRegister( radio, RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO, 0xA1 );

	//WriteRegister( radio, RFREG_CLK_REC_OFFSET_2, 0x20 );
	//WriteRegister( radio, RFREG_CLK_REC_OFFSET_1, 0x4E );
	//WriteRegister( radio, RFREG_CLK_REC_OFFSET_0, 0xA5 );
	//WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_1, 0x00 );
	//WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_0, 0x0A );



	//original values from 4800 bps.

	/* WriteRegister( radio, RFREG_IF_BANDWIDTH, 0x1D );
	WriteRegister( radio, RFREG_AFC_GEARSHIFT_OVERRIDE, 0x40 );
	WriteRegister( radio, RFREG_CLK_RECOVERY_GEARSHIFT_OVERRIDE, 0x03 );
	WriteRegister( radio, RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO, 0xA1 );

	WriteRegister( radio, RFREG_CLK_REC_OFFSET_2, 0x20 );
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_1, 0x4E );
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_0, 0xA5 );
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_1, 0x00 );
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_0, 0x0A );*/
}

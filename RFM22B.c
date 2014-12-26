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

U16 computeTX_DR_forDataRate( U32 dataRate_bps ) {
	U16 txdr = 0x0000;
	if ( dataRate_bps < 30000 ) { // Use txdtrtscale = 1.
		txdr = 2**21 * dataRate_bps;
	} else {
		txdr = 2**16 * dataRate_bps;
	}
	
	return txdr;
}

void setRegisters_forDataRate( U32 dataRate_bps ) {
	U8	txdr_lower = 0x00;
	U8	txdr_upper = 0x00;
	U16	temporary = 0x0000;
	if ( dataRate_bps < 30000 ) { // Use txdtrtscale = 1.
		WriteRegister( radio, RFREG_MOD_MODE_CTRL_1, txdtrtscale );
	} else {
		WriteRegister( radio, RFREG_MOD_MODE_CTRL_1, txdtrtscale );
	}
	
	U16 m_txdr = computeTX_DR_forDataRate( dataRate_bps );
	temporary = m_txdr;
	// Decompose, upper byte goes into txdr_upper.
	temporary = temporary & 0xFF00;
	temporary = temporary >> 8;
	txdr_upper = temporary;
	// Decompose, lower byte goes into txdr_lower.
	temporary = temporary & 0x00ff;
	//temporary = temporary >> 8;
	txdr_lower = temporary;
	//set data rate
	//data rate below 30 kbps. manchester off. data whitening off.
	//WriteRegister( radio, RFREG_TX_DATA_RATE_1, 0x27 ); //txdr[15:8]
	//WriteRegister( radio, RFREG_TX_DATA_RATE_0, 0x52 ); //txdr[7:0]
	WriteRegister( radio, RFREG_TX_DATA_RATE_1, txdr_upper ); //txdr[15:8]
	WriteRegister( radio, RFREG_TX_DATA_RATE_0, txdr_lower ); //txdr[7:0]

	return;
}

void InitRfm22( U8 radio )
{
	// Variables
	// Data rate 1 - 128 kbps.
	const U32 dataRate_bps = 4800;
	// Note: upper was 0x27, lower was 0x52.
	// 0x2752 = 10066, so 4.8kbps.
	// TX_DR = 10**3 * txdr[15:0]/2**16 in Kbps (if register 70(5) == 0)
	// Else "	"	"	 21	"	"	"      == 1)
	

	
	
	//disable all interrupts
	WriteRegister( radio, RFREG_INT_EN_2, 0x00 );

	//enter ready mode (xtal on, PLL off)
	WriteRegister( radio, RFREG_FUNC_CTRL_1, txon ); //xton

	//set crystal oscillator load capacitance to 0x7F. (no idea why)
	WriteRegister( radio, RFREG_LOAD_CAP, 0x7F ); //xlc[6:0]

	/*
	//set data rate
	//data rate below 30 kbps. manchester off. data whitening off.
	WriteRegister( radio, RFREG_MOD_MODE_CTRL_1, txdtrtscale );

	WriteRegister( radio, RFREG_TX_DATA_RATE_1, 0x27 ); //txdr[15:8]
	WriteRegister( radio, RFREG_TX_DATA_RATE_0, 0x52 ); //txdr[7:0]
	*/
	setRegisters_forDataRate( dataRate_bps );
	
	//set frequency
	WriteRegister( radio, RFREG_FREQ_BAND_SEL, sbsel | 0x13 );
	//fb[4:0] = 0x13 = 19 dec
	//sbse = 1 (side band select. no idea what it does)

	WriteRegister( radio, RFREG_NOMINAL_CARRIER_FREQ_1, 0x64 ); //fc[15:8]
	WriteRegister( radio, RFREG_NOMINAL_CARRIER_FREQ_0, 0x00 ); //fc[7:0]

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



	//other / unknown

	//IF Bandwidth filter
	WriteRegister( radio, RFREG_IF_BANDWIDTH, 0x1D );
	WriteRegister( radio, RFREG_AFC_GEARSHIFT_OVERRIDE, 0x40 );
	WriteRegister( radio, RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO, 0xA1 );

	WriteRegister( radio, RFREG_CLK_REC_OFFSET_2, 0x20 );
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_1, 0x4E );
	WriteRegister( radio, RFREG_CLK_REC_OFFSET_0, 0xA5 );
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_1, 0x00 );
	WriteRegister( radio, RFREG_CLK_REC_TIMING_LOOP_GAIN_0, 0x0A );

	WriteRegister( radio, RFREG_MODEM_TEST, 0x01 );

	WriteRegister( radio, RFREG_FREQ_DEV, 0x48 );

	WriteRegister( radio, RFREG_VCO_CURRENT_TRIM, 0x7F );

	WriteRegister( radio, RFREG_DRIVIDER_CURRENT_TRIM, 0x40 );

	WriteRegister( radio, RFREG_CHARGEPUMP_CURRENT_OVERRIDE, 0x80 );

	WriteRegister( radio, RFREG_AGC_OVERRIDE_2, 0x0B );

	WriteRegister( radio, RFREG_DELTA_ADC_TUNE_2, 0x04 );

	WriteRegister( radio, RFREG_CLK_RECOVERY_GEARSHIFT_OVERRIDE, 0x03 );






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


}

void SentTestPacket( U8 radio )
{
	U8 i;

	WriteRegister( radio, RFREG_FUNC_CTRL_1, 0x01);	// To ready mode

	SetAnt( radio, ANT_TX );
	__delay_cycles( 50000 * 8 );

	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x03 );	// FIFO reset
	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x00 );	// Clear FIFO

	WriteRegister(radio, RFREG_PREAMBLE_LEN, 64);	// preamble = 64nibble
	WriteRegister(radio, RFREG_TX_PACKET_LEN, 17);	// packet length = 17bytes
	for (i=0; i<17; i++)
	{
		WriteRegister(radio, RFREG_FIFO_ACCESS, 'h');	// send payload to the FIFO
	}

	WriteRegister(radio, RFREG_INT_EN_1, 0x04);			// enable packet sent interrupt
	i = ReadRegister(radio, RFREG_INT_STATUS_1);		// Read Interrupt status1 register
	i = ReadRegister(radio, RFREG_INT_STATUS_2);

	WriteRegister(radio, RFREG_FUNC_CTRL_1, 9);	// Start TX

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

	WriteRegister(radio, RFREG_FUNC_CTRL_1, 0x01);	// to ready mode

	SetAnt( radio, ANT_OFF );

}

void SetupRecieveTestPacket( U8 radio )
{
	U8 i;

	SetAnt( radio, ANT_RX );
	__delay_cycles( 50000 * 8 );

	WriteRegister( radio, RFREG_FUNC_CTRL_1, 0x01);	// to ready mode

	//clear interrupts.
	i = ReadRegister( radio, RFREG_INT_STATUS_1);
	i = ReadRegister( radio, RFREG_INT_STATUS_2);

	WriteRegister( radio, RFREG_RX_FIFO_CTRL, 17);//fx fifo almost full threshold set to 17.

	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x03);//clear both fifos
	WriteRegister( radio, RFREG_FUNC_CTRL_2, 0x00);//second write in above opperation

	WriteRegister( radio, RFREG_FUNC_CTRL_1, 5);//RX on in manual receiver mode.  auto clears when a valid packet is received. (does nothing in multiple packet config)
	//ready mode
	WriteRegister( radio, RFREG_INT_EN_1, 2);//enable interrupt for valid packet received

}

bool TryRecieveTestPacket( U8 radio )
{
	//check register for interrupt. if a valid packet is not RXed, return false.
	U8 status;
	U8 i;
	status = ReadRegister( radio, RFREG_INT_STATUS_1 );
	if( status & (1 << 1) == 0 )
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

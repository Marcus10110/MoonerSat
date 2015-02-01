/*
 * RFM22B.H
 *
 *  Created on: Dec 20, 2014
 *      Author: Mark
 */

#ifndef RFM22B_H_
#define RFM22B_H_

#include "SatTypes.h"

#define RADIO0	0
#define RADIO1	1
#define ANT_TX	1
#define ANT_RX	2
#define ANT_OFF	3


#define RFREG_DEVICE_TYPE						0x00
#define RFREG_DEVICE_VERSION					0x01
#define RFREG_DEVICE_STATUS						0x02
#define RFREG_INT_STATUS_1						0x03
#define RFREG_INT_STATUS_2						0x04
#define RFREG_INT_EN_1							0x05
#define RFREG_INT_EN_2							0x06
#define RFREG_FUNC_CTRL_1						0x07
#define RFREG_FUNC_CTRL_2						0x08
#define RFREG_LOAD_CAP							0x09
#define RFREG_MCU_OUT_CLK						0x0A
#define RFREG_GPIO0_CONF						0x0B
#define RFREG_GPIO1_CONF						0x0C
#define RFREG_GPIO2_CONF						0x0D
#define RFREG_PORT_CONF							0x0E
#define RFREG_ADC_CONF							0x0F
#define RFREG_ADC_AMP_OFFSET					0x10
#define RFREG_ADC_VAL							0x11
#define RFREG_TEMP_CTRL							0x12
#define RFREG_TEMP_OFFSET						0x13
#define RFREG_WAKEUP_TIMER_PERIOD_1				0x14
#define RFREG_WAKEUP_TIMER_PERIOD_2				0x15
#define RFREG_WAKEUP_TIMER_PERIOD_3				0x16
#define RFREG_WAKEUP_TIMER_VALUE_1				0x17
#define RFREG_WAKEUP_TIMER_VALUE_2				0x18
#define RFREG_LOW_DUTY_MODE_DURATION			0x19
#define RFREG_LOW_BAT_THRESHOLD					0x1A
#define RFREG_BAT_LEVEL							0x1B
#define RFREG_IF_BANDWIDTH						0x1C
#define RFREG_AFC_GEARSHIFT_OVERRIDE			0x1D
#define RFREG_ADC_TIMING_CTRL					0x1E
#define RFREG_CLK_RECOVERY_GEARSHIFT_OVERRIDE	0x1F
#define RFREG_CLK_RECOVERY_OVERSAMPLE_RATIO		0x20
#define RFREG_CLK_REC_OFFSET_2					0x21
#define RFREG_CLK_REC_OFFSET_1					0x22
#define RFREG_CLK_REC_OFFSET_0					0x23
#define RFREG_CLK_REC_TIMING_LOOP_GAIN_1		0x24
#define RFREG_CLK_REC_TIMING_LOOP_GAIN_0		0x25
#define RFREG_RSSI								0x26
#define RFREG_RSSI_THRESHOLD					0x27
#define RFREG_ANT_DIV_1							0x28
#define RFREG_ANT_DIV_2							0x29
#define RFREG_DATA_ACCESS_CTRL					0x30
#define RFREG_EZMAC_STATUS						0x31
#define RFREG_HEADER_CTRL_1						0x32
#define RFREG_HEADER_CTRL_2						0x33
#define RFREG_PREAMBLE_LEN						0x34
#define RFREG_PREAMBLE_DETECT_CTRL				0x35
#define RFREG_SYNC_WORD_3						0x36
#define RFREG_SYNC_WORD_2						0x37
#define RFREG_SYNC_WORD_1						0x38
#define RFREG_SYNC_WORD_0						0x39
#define RFREG_TX_HEADER_3						0x3A
#define RFREG_TX_HEADER_2						0x3B
#define RFREG_TX_HEADER_1						0x3C
#define RFREG_TX_HEADER_0						0x3D
#define RFREG_TX_PACKET_LEN						0x3E
#define RFREG_CHK_HEADER_3						0x3F
#define RFREG_CHK_HEADER_2						0x40
#define RFREG_CHK_HEADER_1						0x41
#define RFREG_CHK_HEADER_0						0x42
#define RFREG_HEADER_EN_3						0x43
#define RFREG_HEADER_EN_2						0x44
#define RFREG_HEADER_EN_1						0x45
#define RFREG_HEADER_EN_0						0x46
#define RFREG_RX_HEADER_3						0x47
#define RFREG_RX_HEADER_2						0x48
#define RFREG_RX_HEADER_1						0x49
#define RFREG_RX_HEADER_0						0x4A
#define RFREG_RX_PACKET_LEN						0x4B
#define RFREG_ANALOG_TEST_BUS					0x50
#define RFREG_DIGITAL_TEST_BUS					0x51
#define RFREG_TX_RANP_CTRL						0x52
#define RFREG_PLL_TUNE_TIME						0x53
#define RFREG_CAL_CTRL							0x55
#define RFREG_MODEM_TEST						0x56
#define RFREG_CHARGEPUMP_TEST					0x57
#define RFREG_CHARGEPUMP_CURRENT_OVERRIDE		0x58
#define RFREG_DRIVIDER_CURRENT_TRIM				0x59
#define RFREG_VCO_CURRENT_TRIM					0x5A
#define RFREG_VCO_CAL_OVVERIDE					0x5B
#define RFREG_SYNTH_TEST						0x5C
#define RFREG_BLOCK_EN_OVERRIDE_1				0x5D
#define RFREG_BLOCK_EN_OVERRIDE_2				0x5E
#define RFREG_BLOCK_EN_OVERRIDE_3				0x5F
#define RFREG_CHAN_FILT_COEF_ADDRESS			0x50
#define RFREG_CHAN_FILT_CELF_VALUE				0x61
#define RFREG_XTAL_CTRL_TEST					0x62
#define RFREG_RC_OSC_COARSE_CAL_OVERRIDE		0x63
#define RFREG_RC_OSC_FINE_CAL_OVERRIDE			0x64
#define RFREG_LDO_CTRL_OVERRIDE					0x65
#define RFREG_LDO_LVL_SETTING					0x66
#define RFREG_DELTA_ADC_TUNE_1					0x67
#define RFREG_DELTA_ADC_TUNE_2					0x68
#define RFREG_AGC_OVERRIDE_1					0x69
#define RFREG_AGC_OVERRIDE_2					0x6A
#define RFREG_GFSO_FIR_COEF_ADDRESS				0x6B
#define RFREG_GFSK_FIR_COEF_VALUE				0x6C
#define RFREG_TX_POWER							0x6D	// Possible datasheet erratum
#define RFREG_TX_DATA_RATE_1					0x6E
#define RFREG_TX_DATA_RATE_0					0x6F
#define RFREG_MOD_MODE_CTRL_1					0x70
#define RFREG_MOD_MODE_CTRL_2					0x71
#define RFREG_FREQ_DEV							0x72
#define RFREG_FREQ_OFFSET_1						0x73
#define RFREG_FREQ_OFFSET_2						0x74
#define RFREG_FREQ_BAND_SEL						0x75
#define RFREG_NOMINAL_CARRIER_FREQ_1			0x76
#define RFREG_NOMINAL_CARRIER_FREQ_0			0x77
#define RFREG_FREQ_HOP_CHAN_SEL					0x79
#define RFREG_FREQ_HOP_STEP_SIZE				0x7A
#define RFREG_TX_FIFO_CTRL_1					0x7C
#define RFREG_TX_FIFO_CTRL_2					0x7D
#define RFREG_RX_FIFO_CTRL						0x7E
#define RFREG_FIFO_ACCESS						0x7F


//RFREG_FUNC_CTRL_1, 0x07
// Description:		Operating Mode and Function Control 1.
// Register:		07h.
// Reset value:		0x01
// All bits are read and write.
#define swres		( 1 << 7 )	// Software Register Reset Bit.
#define enlbd		( 1 << 6 )	// Enable Low Battery Detect.
#define enwt		( 1 << 5 )	// Enable Wake-Up-Timer.
#define x32ksel		( 1 << 4 )	// 32,768 kHz Crystal Oscillator Select.
#define txon		( 1 << 3 )	// TX on in Manual Transmit Mode.
#define rxon		( 1 << 2 )	// RX on in Manual Receiver Mode.
#define pllon		( 1 << 1 )	// TUNE Mode (PLL is ON).
#define xton		( 1 << 0 )	// READY Mode (Xtal is ON).


//RFREG_DATA_ACCESS_CTRL, 0x30
#define crc_0_ccitt 0x00
#define crc_1_crc_16_ibm 0x01
#define crc_2_iec_16 0x02
#define crc_3_biacheva 0x03
#define encrc ( 1 << 1 )
#define enpactx (1 << 3 )
#define crcdonly ( 1 << 5 )
#define lsbfrst ( 1 << 6 )
#define enpacrx ( 1 << 7 )


//RFREG_TX_POWER, 0x6D
// Description:		TX Power.
// Register:		6Dh.
// Reset value:		0x00001010
// All bits are read and write.


//RFREG_TX_DATA_RATE_1, 0x6E
// Description:		TX Data Rate 1.
// Register:		6Eh.
// Reset value:		0x00001010		// Defaults = 40 kbps.



//RFREG_TX_DATA_RATE_0, 0x6F
// Description:		TX Data Rate 0.
// Register:		6Fh.
// Reset value:		0x00001101		// Defaults = 40 kbps.
// All bits are read and write.



//RFREG_MOD_MODE_CTRL_1, 0x70
#define txdtrtscale ( 1 << 5 ) //This bit should be set for Data Rates below 30 kbps
#define enphpwdn ( 1 << 4 ) //If set, the Packet Handler will be powered down when module is in low power mode
#define manppol ( 1 << 3 ) //Manchester Preamble Polarity (will transmit a series of 1 if set, or series of 0 if reset).
#define enmaninv ( 1 << 2 ) //Manchester Data Inversion is Enabled if this bit is set.
#define enmanch ( 1 << 1 ) //Manchester Coding is Enabled if this bit is set.
#define enwhite ( 1 << 0 ) //Data Whitening is Enabled if this bit is set

//RFREG_MOD_MODE_CTRL_2, 0x71
#define modtyp_0_carrier ( 0 )
#define modtyp_1_ook ( 1 )
#define modtyp_2_fsk ( 2 )
#define modtyp_3_gfsk ( 3 )
#define fd_8 ( 1 << 2 )
#define eninv ( 1 << 3 )
#define dtmod_0_direct_tx_gpio ( 0 << 4 )
#define dtmod_1_direct_tx_sdi ( 1 << 4 )
#define dtmod_2_fifo ( 2 << 4 )
#define dtmod_3_pn9 ( 3 << 4 )
#define trclk_0_no_tx_clk ( 0 << 6 )
#define trclk_1_tx_clk_gpio ( 1 << 6 )
#define trclk_2_tx_clk_sdo ( 2 << 6 )
#define trclk_3_tx_clk_irq ( 3 << 6 )

//RFREG_FREQ_BAND_SEL, 0x75
#define hbsel ( 1 << 5 )
#define sbsel ( 1 << 6 )





void InitRfm22( U8 radio );

void ConfigReadBackTest( U8 radio );

void SentTestPacket( U8 radio );



void SetupRecieveTestPacket( U8 radio );

bool TryRecieveTestPacket( U8 radio );

U16 ComputeTxRateReg( U32 dataRate_bps );
void SetDataRate( U8 radio, U32 dataRate_bps );

void ConfigureRxModemSettings( U8 radio, U32 data_rate_bps, U32 frequency_dev_hz );


#endif /* RFM22B_H_ */

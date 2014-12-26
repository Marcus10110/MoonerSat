#include <msp430.h> 
#include "SatTypes.h"
#include "RFM22B.h"
#include "SatPeripherals.h"
#include "SatDefines.h"
/*
 * main.c
 */


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //TODO: set clock source
    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_6;                       // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;  // Set SMCLK = MCLK = DCO
                                              // ACLK = VLOCLK
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers to 1
    CSCTL0_H = 0;                             // Lock CS registers

    //init IO pins.
    InitIo();

    //init SPI module
    InitSpi();

    __delay_cycles( 4000000 );
    //init radios
    InitRfm22(RADIO0);
    __delay_cycles(8000);
    ConfigReadBackTest(RADIO0);
    __delay_cycles(100000);
    InitRfm22(RADIO1);
    __delay_cycles(8000);
    ConfigReadBackTest(RADIO1);
    __delay_cycles(100000);

    //testing SPI.

    while(1)
    {
    	U8 i = 0;

		/*RF0_CS_ON;
		ReadWriteSpi( 0x1A );
		ReadWriteSpi( 0x52 );
		RF0_CS_OFF;

		__delay_cycles(1000);

		RF1_CS_ON;
		ReadWriteSpi( 0x0F );
		ReadWriteSpi( 0xAA );
		RF1_CS_OFF;

		__delay_cycles(1000);*/

    	__delay_cycles( 4000000 );

    	SetupRecieveTestPacket( RADIO1 );

    	__delay_cycles( 80000 );//10ms

    	SentTestPacket( RADIO0 );

    	__delay_cycles( 8000 );//1ms

    	for( i = 0; i < 10; ++i)
    	{
    		bool success;
    		success = TryRecieveTestPacket( RADIO1 );

    		if( success == TRUE )
    			break;

    		__delay_cycles( 8000 ); //1ms


    	}



    }


    //test modems here.

	return 0;
}

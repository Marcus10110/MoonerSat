#include <msp430.h> 
#include "SatTypes.h"
#include "RFM22B.h"
#include "SatDefines.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"


/*
 * main.c
 */

gpio_t rfm0_cs = { RF0_CS_PORT, RF0_CS_PIN };
gpio_t rfm1_cs = { RF1_CS_PORT, RF1_CS_PIN };

gpio_t rfm0_tx_pin = { RF0_TXANT_PORT, RF0_TXANT_PIN };
gpio_t rfm0_rx_pin = { RF0_RXANT_PORT, RF0_RXANT_PIN };

gpio_t rfm1_tx_pin = { RF1_TXANT_PORT, RF1_TXANT_PIN };
gpio_t rfm1_rx_pin = { RF1_RXANT_PORT, RF1_RXANT_PIN };

spi_t spi_0;
spi_t spi_1;

rfm22b_t rfm_0;
rfm22b_t rfm_1;

uart_t uart;


int main(void)
{
	U8 tx_buffer[12];
	U8 rx_buffer[12];

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //TODO: set clock source
    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_6;                       // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;  // Set SMCLK = MCLK = DCO
                                              // ACLK = VLOCLK
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers to 1
    CSCTL0_H = 0;                             // Lock CS registers


    gpio_setup();
    spi_setup();
    rfm_setup();
    uart_setup();


    gpio_init( &rfm0_cs );
    gpio_init( &rfm1_cs );
    gpio_init( &rfm0_tx_pin );
    gpio_init( &rfm0_rx_pin );
    gpio_init( &rfm1_tx_pin );
    gpio_init( &rfm1_rx_pin );


    spi_init( &spi_0, &rfm0_cs );
    spi_init( &spi_1, &rfm1_cs );


    uart_init( &uart );

    rfm_init( &rfm_0, &spi_0, &rfm0_tx_pin, &rfm0_rx_pin );
    rfm_init( &rfm_1, &spi_1, &rfm1_tx_pin, &rfm1_rx_pin );

    rfm_set_baud(&rfm_0, BAUD_9_6K_FDEV_45K);
    rfm_set_frequency(&rfm_0, 434000000);
    rfm_set_power(&rfm_0, 10);

    rfm_set_baud(&rfm_1, BAUD_9_6K_FDEV_45K);
	rfm_set_frequency(&rfm_1, 434000000);
	rfm_set_power(&rfm_1, 10);

	uart_set_baud( &uart, 9600 );



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

    	rfm_start_rx( &rfm_1 );

    	rfm_tx_data( &rfm_0, (U8*)"hello world!", 12 );


    	for( i = 0; i < 10; ++i)
    	{
    		bool success;
    		rfm_is_data_availible( &rfm_1, &success );

    		if( success == TRUE )
    		{
    			rfm_rx_data( &rfm_1, rx_buffer, 12 );
    			break;
    		}


    		__delay_cycles( 8000 ); //1ms

    	}



    }

}

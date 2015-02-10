#include <msp430.h> 
#include "SatTypes.h"
#include "RFM22B.h"
#include "SatDefines.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "i2c.h"

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

i2c_t i2c;

rfm22b_t rfm_0;
rfm22b_t rfm_1;

uart_t uart;


int main(void)
{
	U8 tx_buffer[12];
	U8 rx_buffer[12];
	bool serial_comm_mode = FALSE;

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
    spi_setup( SPI_UCA1 );
    rfm_setup();
    uart_setup();
    i2c_setup();

    gpio_init( &rfm0_cs );
    gpio_init( &rfm1_cs );
    gpio_init( &rfm0_tx_pin );
    gpio_init( &rfm0_rx_pin );
    gpio_init( &rfm1_tx_pin );
    gpio_init( &rfm1_rx_pin );


    spi_init( &spi_0, SPI_UCA1, &rfm0_cs );
    spi_init( &spi_1, SPI_UCA1, &rfm1_cs );

    i2c_init( &i2c );

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
    	U8 j = 0;
    	bool ready = FALSE;
    	U8 rx_serial_byte;
    	U8 tx_count = 12;



    	__delay_cycles( 4000000 );

    	if( serial_comm_mode == FALSE )
    	{
    		uart_is_byte_availible( &uart, &ready );
    		if( ready == TRUE )
			{
				uart_read_byte( &uart, &rx_serial_byte );
				serial_comm_mode = TRUE;
			}

    	}
    	else if( serial_comm_mode == TRUE )
    	{
    		tx_count = BlockForCommand( tx_buffer, 12 );
    	}


    	rfm_start_rx( &rfm_1 );

    	if( serial_comm_mode == FALSE )
    	{
    		rfm_tx_data( &rfm_0, (U8*)"hello world!", 12 );
    	}
    	else
    	{
    		rfm_tx_data( &rfm_0, tx_buffer, tx_count );
    	}

    	for( i = 0; i < 10; ++i)
    	{
    		bool success;
    		rfm_is_data_availible( &rfm_1, &success );

    		if( success == TRUE )
    		{
    			rfm_rx_data( &rfm_1, rx_buffer, tx_count );

    			for( j = 0; j < tx_count; ++j )
    				uart_write_byte( &uart, rx_buffer[j] );
    			break;
    		}


    		__delay_cycles( 8000 ); //1ms
    	}
    }
}

U8 BlockForCommand( U8* buffer, U8 max_count ) //returns count.
{
	bool ready = FALSE;
	U8 rx_serial_byte;
	U8 count = 0;

	while(TRUE)
	{
		uart_is_byte_availible( &uart, &ready );


		if( ready == TRUE )
		{
			uart_read_byte( &uart, &rx_serial_byte );
		}
		else
		{
			continue;
		}

		if( rx_serial_byte == '\r' )
		{
			return count;
		}

		buffer[count] = rx_serial_byte;
		count++;
		if( count >= max_count )
			return count;
	}




}

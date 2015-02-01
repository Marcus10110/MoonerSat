/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2011 Fergus Noble <fergusnoble@gmail.com>
 * Copyright (C) 2011 Henry Hallam <henry@pericynthion.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <errno.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

int _write(int file, char *ptr, int len);

static void clock_setup(void)
{
    /* Enable clocks on all the peripherals we are going to use. */
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
}

static void spi_setup(void)
{
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
            GPIO13 | GPIO14 | GPIO15);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);

    /* Setup GPIO3 (in GPIO port C) as chip select (active-low). */
    gpio_set(GPIOB, GPIO12);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO12);

    /* Setup SPI parameters. */
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_256, SPI_CR1_CPOL,
            SPI_CR1_CPHA, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
    spi_enable_ss_output(SPI2); /* Required, see NSS, 25.3.1 section. */

    /* Finally enable the SPI. */
    spi_enable(SPI2);
}

static void gpio_setup(void)
{
    gpio_set(GPIOC, GPIO3);

    /* Setup GPIO3 (in GPIO port C) for LED use. */
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_MODE_OUTPUT, GPIO3);
}

int main(void)
{
    uint8_t counter = 0;

    clock_setup();
    gpio_setup();
    spi_setup();

    while (1) {
        for( int i = 0; i < 1025; i++ ) { counter++; } /* Wait grand total +1 */
        gpio_clear(GPIOB, GPIO12);
        spi_xfer(SPI2, counter);
        gpio_set(GPIOB, GPIO12);
    }

    return 0;
}

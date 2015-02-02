/* Stm32SatDefines.h - shim in stm32 pin changes, for now */

#ifndef STM32_SAT_DEFINES_H_
#define STM32_SAT_DEFINES_H_

#include <libopencm3/stm32/gpio.h>

/* Terrible hacks, preprocessor substitutions for gpio set/clear chip select */
// Dont use radio 1
#define RF1_CS_ON
#define RF1_CS_OFF
#define RF1_RXANT_ON
#define RF1_RXANT_OFF
#define RF1_TXANT_ON
#define RF1_TXANT_OFF

// Use radio2
#define RF0_CS_ON       gpio_clear(GPIOB, GPIO12);
#define RF0_CS_OFF      gpio_set(GPIOB, GPIO12);
#define RF0_RXANT_ON    gpio_set(GPIOB, GPIO1);
#define RF0_RXANT_OFF   gpio_clear(GPIOB, GPIO1);
#define RF0_TXANT_ON    gpio_set(GPIOB, GPIO0);
#define RF0_TXANT_OFF   gpio_clear(GPIOB, GPIO0);

#endif // !defined STM32_SAT_DEFINES_H_

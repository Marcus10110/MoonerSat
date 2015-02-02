/* systick.h - system tick sleep for stm32 */

#ifndef STM32_SYSTICK_H_
#define STM32_SYSTICK_H_

#include <stdint.h>

// Call this with other setup()s in main() before starting
void systick_setup(void);
// Call this to sleep for a given number of milliseconds
void msleep(uint32_t delay /* milliseconds */);
// OH GOD WHAT A TERRIBLE HACK
void usleep(uint32_t barf);

#endif // !defined STM32_SYSTICK_H_

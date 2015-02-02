/* systick.h - system tick sleep for stm32 */

// Call this with other setup()s in main() before starting
void systick_setup(void);
// Call this to sleep for a given number of milliseconds
void msleep(uint32_t delay /* milliseconds */);

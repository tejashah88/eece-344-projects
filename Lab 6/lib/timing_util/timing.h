#ifndef MCU_TIMING_UTIL
#define MCU_TIMING_UTIL

#include <stdint.h>


// Wait for delay x 1 us via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
extern void SysTick_Wait_1us(uint32_t delay);


// Wait for delay x 1 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
extern void SysTick_Wait_1ms(uint32_t delay);


// Wait for delay x 500 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
extern void SysTick_Wait_500ms(uint32_t delay);


#endif /* MCU_TIMING_UTIL */

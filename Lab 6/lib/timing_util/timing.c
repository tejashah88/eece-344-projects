#include <stdint.h>

#include "systick/SysTick.h"
#include "timing.h"

// Wait for delay x 1 us via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_1us(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 50 * 20 ns
        SysTick_Wait(50);
    }
}

// Wait for delay x 1 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_1ms(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 50,000 * 20 ns
        SysTick_Wait(50000);
    }
}

// Wait for delay x 500 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_500ms(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 25,000,000 * 20 ns
        SysTick_Wait(25000000);
    }
}

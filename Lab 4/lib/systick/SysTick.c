#include <stdint.h>
#include "mcu/tm4c123gh6pm.h"
#include "SysTick.h"

// Configure SysTick with busy wait running at initialized bus clock (from PLL)
// Source: https://web2.qatar.cmu.edu/cs/15348/lectures/Lecture08.pdfs
// NOTE: Accessing the RELOAD register requires system clock to be greater than 8 MHz
void SysTick_Init(void) {
    // Disable SysTick during setup
    NVIC_ST_CTRL_R = 0;

    // Set default reload value to maximum reload value
    NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;

    // Set current value of counter to 0 to trigger reload after first cycle
    NVIC_ST_CURRENT_R = 0;

    // Re-enable SysTick with source set to system clock
    NVIC_ST_CTRL_R = (NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC);
}

// Unit time delay based on system clock
// Source: https://web2.qatar.cmu.edu/cs/15348/lectures/Lecture08.pdfs
// NOTE: It's better to construct higher level functions with greater delays to reduce function calling overhead
// Delay parameter units is 1 per cycle time
// Examples:
// - 80 Mhz    => t = 1 / 80 Mhz    =  12.5 ns (unit)
// - 50 Mhz    => t = 1 / 50 Mhz    =  20.0 ns (unit)
// - 40 Mhz    => t = 1 / 40 Mhz    =  25.0 ns (unit)
// - 3.125 Mhz => t = 1 / 3.125 Mhz = 320.0 ns (unit)
void SysTick_Wait(uint32_t delay) {
    // Set reload value to number of counts to wait
    // NOTE: Subtracting by 1 since reload will be triggered by first cycle
    NVIC_ST_RELOAD_R = delay - 1;

    // Set current value of counter to 0 to trigger reload after first cycle
    NVIC_ST_CURRENT_R = 0;

    // Wait for count bit to be set to 1, otherwise keep checking
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0);
}

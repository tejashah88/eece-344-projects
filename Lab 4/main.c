/*
 * Reference material:
 * - MCU Datasheet: https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf
 * - Suplimental info: https://web2.qatar.cmu.edu/cs/15348/lectures/Lecture03.pdf
 *
 * Useful info:
 * - Pin to hex: (1 << pinN)
 * - Bit manipulation:
 *   - Bit set: (arg |= (1 << pinN))
 *   - Bit clear: (arg &= ~(1 << pinN))
 *   - Bit toggle: (arg ^= (1 << pinN))
 *   - Bit test: (arg & (1 << pinN) == 1)
 */

#include <stdint.h>

#include "lib/mcu/tm4c123gh6pm.h"
#include "lib/pll/PLL.h"
#include "lib/systick/SysTick.h"

// Task pin definitions
#define OUTPUT_INDICATOR_LEDS 0x38u // = 0x20 (PA5) | 0x10 (PA4) | 0x08 (PA3)
#define INPUT_DOOR_BUTTONS    0xC0u // = 0x80 (PA7) | 0x40 (PA6)
#define PORTA_WORKING_PINS    0xF8u // = 0x80 (PA7) | 0x40 (PA6) | 0x20 (PA5) | 0x10 (PA4) | 0x08 (PA3)


// Function Declarations
void Setup_Port_A_Pins(void);
void SysTick_Wait_200ms(uint32_t delay);

//////////////////////////
// GPIO Setup functions //
//////////////////////////


void Setup_Port_A_Pins(void) {
    // Enable Port A clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // Wait until Port A clock is fully initialized
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0);

    // Unlock Port A configuration
    GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with PA7 - PA3
    GPIO_PORTA_CR_R = PORTA_WORKING_PINS;

    // Set output indicator LEDs (PA7 & PA6) pin as output and rest as inputs
    GPIO_PORTA_DIR_R |= OUTPUT_INDICATOR_LEDS;

    // Disable all analog functionality for PA7 - PA3
    GPIO_PORTA_AMSEL_R &= ~PORTA_WORKING_PINS;

    // Enable digital functionality for PA7 - PA3
    GPIO_PORTA_DEN_R |= PORTA_WORKING_PINS;

    // Disable all alternate functionality for PA7 - PA3
    GPIO_PORTA_AFSEL_R &= ~PORTA_WORKING_PINS;

    // Disable all special functionality for PA7 - PA3
    GPIO_PORTA_PCTL_R &= ~PORTA_WORKING_PINS;

    // Enable pull-down resistors for PA7 & PA6 (active high logic)
    GPIO_PORTA_PDR_R |= INPUT_DOOR_BUTTONS;

    // Lock Port A configuration
    GPIO_PORTA_LOCK_R = 0;
}


//////////////////////
// Timing Functions //
//////////////////////


// Wait for delay x 200 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_200ms(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 10,000,000 * 20 ns
        SysTick_Wait(10000000);
    }
}

/////////////////////
// State definiton //
/////////////////////

struct State {
    // Output to set for LED indicator pins (PA3 - PA5)
    unsigned long Output;
    // Amount of time x 1 second to wait before next state transition
    unsigned long Time;
    // Next state transition as specified by input from (PA7 | PA6)
    struct State *Next[4];
};

typedef struct State SType;

SType FSM[4];
#define empty &FSM[0]
#define enter &FSM[1]
#define hold  &FSM[2]
#define leave &FSM[3]

// See state transition document for more information
SType FSM[4] = {
    {0x00, 1, {empty, empty, enter, empty}},
    {0x20, 2, {empty, hold , enter, hold }},
    {0x10, 4, {leave, leave, leave, leave}},
    {0x08, 3, {empty, leave, empty, leave}},
};


//////////////////
// Main program //
//////////////////


int main() {
    // Define state variables & set initial state to 'empty'
    unsigned long Input;
    SType *Pt;
    Pt = empty;

    // Initialize PLL & SysTick
    PLL_Init(SYSDIV2_50_00_Mhz);
    SysTick_Init();

    // Initialize GPIO pins
    Setup_Port_A_Pins();

    while (1) {
        // Set LED indicators based on current state
        GPIO_PORTA_DATA_R = Pt -> Output;

        // Wait for specified amount of time (in seconds)
        SysTick_Wait_200ms((Pt -> Time) * 5);

        // Read from door input buttons
        Input = (GPIO_PORTA_DATA_R & 0xC0) >> 6;

        // Transition to next state based on input
        Pt = Pt -> Next[Input];
    }
}

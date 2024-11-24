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
#include <math.h>

#include "lib/mcu/tm4c123gh6pm.h"
#include "lib/pll/PLL.h"
#include "lib/systick/SysTick.h"
#include "lib/nvic/Interrupt.h"

// Task pin definitions
#define INPUT_BUTTON_PINS	0x11u // = 0x10 (PF4) | 0x01 (PF0)
#define OUTPUT_DAC_PIN		0xFFu // = 0x80 (PB7) | 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)

// Debug flags
#define TEST_DAC_OUTPUTS 0  // Controls if the DEBUG stair voltage waveform is to be generated (see relevant function for more info)

// Constant definitions
#define SAWTOOTH_PERIOD 256
#define SINE_PERIOD 60


// Function Declarations
void Setup_Port_B_Pins(void);
void Setup_Port_F_Pins(void);
void Setup_Global_Interrupts(void);
void SysTick_Wait_1ms(uint32_t delay);

uint8_t DEBUG_Generate_Stair_Voltage_Waveform_Tick(int tick);
uint8_t Generate_Sawtooth_Waveform_Tick(int tick);
uint8_t Generate_Sine_Waveform_Tick(int tick);


//////////////////////
// Global Variables //
//////////////////////


// NOTE: Declaring a global variable 'volatile' flags it to the compiler that this variable
// may be changed outside the obviously defined program flow (useful for interrupt handling).
volatile int outputMode = 0;


//////////////////////////
// GPIO Setup functions //
//////////////////////////


void Setup_Port_B_Pins(void) {
    // Enable Port B clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Wait until Port B clock is fully initialized
    while ((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R1) == 0);

    // Unlock Port B configuration
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PB0 - PB7
    GPIO_PORTB_CR_R = OUTPUT_DAC_PIN;

    // Set DAC output pins (PB0 - PB7) as outputs
    GPIO_PORTB_DIR_R |= OUTPUT_DAC_PIN;

    // Disable analog functionality for PB0 - PB7
    GPIO_PORTB_AMSEL_R &= ~(OUTPUT_DAC_PIN);

    // Enable digital functionality for PB0 - PB7
    GPIO_PORTB_DEN_R |= OUTPUT_DAC_PIN;

    // Disable all alternate functionality for PB0 - PB7
    GPIO_PORTB_AFSEL_R &= ~OUTPUT_DAC_PIN;

    // Disable all special functionality for PB0 - PB7
    GPIO_PORTB_PCTL_R &= ~OUTPUT_DAC_PIN;

    // Lock Port B configuration
    GPIO_PORTB_LOCK_R = 0;
}


void Setup_Port_F_Pins(void) {
    // Enable Port F clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    // Wait until Port F clock is fully initialized
    while ((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R5) == 0);

    // Unlock Port F configuration
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PF0 & PF4
    GPIO_PORTF_CR_R = INPUT_BUTTON_PINS;

    // Set input buttons pins (PF0 & PF4) as inputs
    GPIO_PORTF_DIR_R &= ~INPUT_BUTTON_PINS;

    // Disable analog functionality for PF0 & PF4
    GPIO_PORTF_AMSEL_R &= ~(INPUT_BUTTON_PINS);

    // Enable digital functionality for PF0 & PF4
    GPIO_PORTF_DEN_R |= INPUT_BUTTON_PINS;

    // Disable all alternate functionality for PF0 & PF4
    GPIO_PORTF_AFSEL_R &= ~INPUT_BUTTON_PINS;

    // Disable all special functionality for PF0 & PF4
    GPIO_PORTF_PCTL_R &= ~INPUT_BUTTON_PINS;

    // Enable pull-down resistors for input button pins (PF0 & PF4)
    GPIO_PORTF_PUR_R |= INPUT_BUTTON_PINS;

    //////////////////////////////
    // Interrupt-specific Setup //
    //////////////////////////////

    // Set interrupt trigger for PF0 & PF4 to be edge sensitive
    GPIO_PORTF_IS_R &= ~INPUT_BUTTON_PINS;

    // Set interrupt trigger for PF0 & PF4 to NOT be fired from both edges
    GPIO_PORTF_IBE_R &= ~INPUT_BUTTON_PINS;

    // Set PF0 & PF4 to listen to falling edge
    GPIO_PORTF_IEV_R &= ~INPUT_BUTTON_PINS;

    // Clear any prior interrupts for PF0 & PF4
    GPIO_PORTF_ICR_R |= INPUT_BUTTON_PINS;

    // Unmask the interrupt for PF0 & PF4
    GPIO_PORTF_IM_R |= INPUT_BUTTON_PINS;

    // Lock Port F configuration
    GPIO_PORTF_LOCK_R = 0;
}


////////////////////////////
// Global Interrupt Setup //
////////////////////////////


void Setup_Global_Interrupts(void) {
    // Set interrupt priority for Interrupt #30 (for GPIO Port F) to level 5 (Refer to Table 2-9 or pg. 104-106 in datasheet)
    // Equivalent statement: NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (5 << 21);
    NVIC_SetPriorityIRQn(30, 5);

    // Enable interrupt #30 (for GPIO Port F) (Refer to Table 2-9 or pg. 104-106 in datasheet)
    // Equivalent statement: NVIC_EN0_R |= (1 << 30);
    NVIC_EnableIRQn(30);

    // Globally enable interrupt requests (IRQs) by clearing priority mask
    // Equivalent assembly statement: "CPSIE I" (Change Processor State Interrupt Enable)
    __enable_irq();
}


//////////////////////
// Timing Functions //
//////////////////////


// Wait for delay x 1 ms via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_1ms(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 50,000 * 20 ns
        SysTick_Wait(50000);
    }
}


////////////////////////
// Interrupt Handlers //
////////////////////////


// NOTE: This is meant to be implicitly overriden
void GPIOF_Handler(void) {
    volatile int readback;

    while (GPIO_PORTF_MIS_R != 0) {
        // NOTE: You cannot have checks on multiple pins in 1 statement or it will not work
        //   (i.e. GPIO_PORTF_MIS_R & 0x11 will NOT work as expected)
        if ((GPIO_PORTF_MIS_R & 0x10) && (GPIO_PORTF_MIS_R & 0x01)) { // Have both SW1 (PF4) and SW2 (PF0) been pressed?
            // Don't display anything
            outputMode = 0;

            // Clear the PF4 & PF0 interrupt flags, making sure to force clear it via a read
            GPIO_PORTF_ICR_R |= 0x11;
            readback = GPIO_PORTF_ICR_R;
        } else if (GPIO_PORTF_MIS_R & 0x10) { // Has SW1 (PF4) been pressed?
            // Start outputting a sawtooth waveform
            outputMode = 1;

            // Clear the PF4 interrupt flag, making sure to force clear it via a read
            GPIO_PORTF_ICR_R |= 0x10;
            readback = GPIO_PORTF_ICR_R;
        } else if (GPIO_PORTF_MIS_R & 0x01) { // Has SW2 (PF0) been pressed?
            // Start outputting a sine waveform
            outputMode = 2;

            // Clear the PF0 interrupt flag, making sure to force clear it via a read
            GPIO_PORTF_ICR_R |= 0x01;
            readback = GPIO_PORTF_ICR_R;
        }
    }
}


/////////////////////////
// Waveform Generation //
/////////////////////////


// Debug function to verify the voltages for each segment of the DAC circuit
// Output is a stair-like waveform, with each level shown on the oscilloscope per 60 milliseconds
uint8_t DEBUG_Generate_Stair_Voltage_Waveform_Tick(int tick) {
    unsigned char t = (unsigned char) (tick / 60) % 8;
    return (1 << t);
}


// Generates a sawtooth / right-triangle waveform, starting from 0 and rising to -3.3 V before repeating
uint8_t Generate_Sawtooth_Waveform_Tick(int tick) {
    // Resulting output range: [0, +255]
    unsigned char y_output = (unsigned char) (tick % SAWTOOTH_PERIOD);

    // HACK: For some reason, our DAC implementation was such that a 1/2 scaling factor was needed to
    // prevent clipping, might be due to op-amp voltage power supply levels despite being set to +/- 5V...
    // NOTE: If not needed, comment the following line out

    // Resulting output range: [0, +127]
    y_output = y_output / 2;

    return y_output;
}


// Generates a sinusoidal waveform with a range from +0 V to -3.3 V and repeating a cycle every 60 ms
uint8_t Generate_Sine_Waveform_Tick(int tick) {
    // Formula: y = sin(2*pi*(t / T)), where T = length of full period
    unsigned char t = (unsigned char) (tick % SINE_PERIOD);
    unsigned char y_output;
    float y, y_shifted, y_scaled;

    // Resulting output range: [-1, +1]
    y = sinf(2 * 3.14 * t / SINE_PERIOD);

    // Resulting output range: [0, +2]
    y_shifted = y + 1;

    // Resulting output range: [0, +1]
    y_scaled = y_shifted / 2;

    // Resulting output range: [0, +255]
    y_output = (unsigned char) (y_scaled * 255);

    // HACK: For some reason, our DAC implementation was such that a 1/2 scaling factor was needed to
    // prevent clipping, might be due to op-amp voltage power supply levels despite being set to +/- 5V...
    // NOTE: If not needed, comment the following line out

    // Resulting output range: [0, +127]
    y_output = y_output / 2;

    return y_output;
}


int main() {
    int tick = 0;
    uint8_t dac_output;

    // Initialize PLL & SysTick
    PLL_Init(SYSDIV2_50_00_Mhz);
    SysTick_Init();

    // Initialize GPIO for Ports B & F
    Setup_Port_B_Pins();
    Setup_Port_F_Pins();

    // Setup global interrupts for GPIO Port F
    Setup_Global_Interrupts();

    while (1) {
        #if TEST_DAC_OUTPUTS
            // If the 'TEST_DAC_OUTPUTS' flag is set to 1, then we don't care about button inputs for the time being
            // This function is meant for debugging the voltage outputs for every segment of the DAC individually
            // If not desired, make sure to set to 0 and re-compile
            dac_output = DEBUG_Generate_Stair_Voltage_Waveform_Tick(tick % 480);
        #else
            // Normal operation: The waveform generation works similar to a game engine by displaying the state of
            // the waveform per tick. This allows as close to realtime switching of the waveform as you can get, but
            // it also has the side-effect of seeing partial waveforms if the buttons are pressed during the middle
            // of a waveform cycle. This is really meant to demonstrate the realtime responsiveness of the interrupts.
            switch (outputMode) {
                case 0:
                    dac_output = 0x00;
                    break;
                case 1:
                    dac_output = Generate_Sawtooth_Waveform_Tick(tick % 256);
                    break;
                case 2:
                    dac_output = Generate_Sine_Waveform_Tick(tick % 60);
                    break;
                default:
                    // NOTE: This should never happen but better safe than sorry
                    dac_output = 0x00;
                    break;
            }
        #endif

        // Write the DAC output from the waveform generation directly to all pins on Port B
        // NOTE: This assumes that PB7 is the MSB and PB0 is the LSB
        GPIO_PORTB_DATA_R = dac_output;

        // Make sure to reset the counter to 0 after every 3840 ms
        // - 3840 / 256      = 15 full cycles of sawtooth waveform
        // - 3840 / 60       = 64 full cycles of sine waveform
        // - 3840 / (8 * 60) = 8 full cucles of DEBUG stair voltage waveform
        tick = (tick + 1) % 3840;

        // Delay for exactly 1 millisecond due to waveform generation logic
        SysTick_Wait_1ms(1);
    }
}

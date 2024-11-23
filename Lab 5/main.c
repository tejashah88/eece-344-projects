/*
 * Reference material:
 * - MPU Datasheet: https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf
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

// GPIO Definitions
#define INPUT_BUTTON_PINS	0x11u // = 0x10 (PF4) | 0x01 (PF0)
#define OUTPUT_DAC_PIN		0xFFu // = 0x80 (PB7) | 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)


// Function Declarations
void setup_port_f_pins(void);
void setup_port_b_pins(void);
void setup_global_interrupts(void);

void generate_sawtooth_waveform_tick(int tick);
void generate_sine_waveform_tick(int tick);
void write_dac_output(unsigned char input);


//////////////////////
// Global Variables //
//////////////////////

// NOTE: Declaring a global variable 'volatile' flags it to the compiler that this variable
// may be changed outside the obviously defined program flow (useful for interrupt handling).
volatile int outputMode = 0;

/////////////////////
// Setup functions //
/////////////////////


void setup_port_b_pins(void) {
    /* Enable Port B clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    /* Wait until Port B clock is fully initialized */
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R1) == 0);

    /* Unlock Port B configuration */
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;

    /* Set commit register to only work with pins PB0 - PB7 */
    GPIO_PORTB_CR_R = OUTPUT_DAC_PIN;

    /* Set DAC output pins (PB0 - PB7) as outputs */
    GPIO_PORTB_DIR_R |= OUTPUT_DAC_PIN;

    /* Disable analog functionality for PB0 - PB7 */
    GPIO_PORTB_AMSEL_R &= ~(OUTPUT_DAC_PIN);

    /* Enable digital functionality for PB0 - PB7 */
    GPIO_PORTB_DEN_R |= OUTPUT_DAC_PIN;

    /* Disable all alternate functionality for PB0 - PB7 */
    GPIO_PORTB_AFSEL_R &= ~OUTPUT_DAC_PIN;

    /* Disable all special functionality for PB0 - PB7 */
    GPIO_PORTB_PCTL_R &= ~OUTPUT_DAC_PIN;

    /* Lock Port B configuration */
    GPIO_PORTB_LOCK_R = 0;
}


void setup_port_f_pins(void) {
    /* Enable Port F clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;

    /* Wait until Port F clock is fully initialized */
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R5) == 0);

    /* Unlock Port F configuration */
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;

    /* Set commit register to only work with pins PF0 & PF4 */
    GPIO_PORTF_CR_R = INPUT_BUTTON_PINS;

    /* Set input buttons pins (PF0 & PF4) as inputs */
    GPIO_PORTF_DIR_R &= ~INPUT_BUTTON_PINS;

    /* Disable analog functionality for PF0 & PF4 */
    GPIO_PORTF_AMSEL_R &= ~(INPUT_BUTTON_PINS);

    /* Enable digital functionality for PF0 & PF4 */
    GPIO_PORTF_DEN_R |= INPUT_BUTTON_PINS;

    /* Disable all alternate functionality for PF0 & PF4 */
    GPIO_PORTF_AFSEL_R &= ~INPUT_BUTTON_PINS;

    /* Disable all special functionality for PF0 & PF4 */
    GPIO_PORTF_PCTL_R &= ~INPUT_BUTTON_PINS;

    /* Enable pull-down resistors for input button pins (PF0 & PF4) */
    GPIO_PORTF_PUR_R |= INPUT_BUTTON_PINS;

    /////////////////////
    // Interrupt Setup //
    /////////////////////

    // Set PF0 & PF4 to be edge sensitive
    GPIO_PORTF_IS_R  &= ~INPUT_BUTTON_PINS;

    // Set trigger to NOT be fired from both edges
    GPIO_PORTF_IBE_R &= ~INPUT_BUTTON_PINS;

    // Set pins to listen to the falling edge
    GPIO_PORTF_IEV_R &= ~INPUT_BUTTON_PINS;

    // Clear any prior interrupts
    GPIO_PORTF_ICR_R |= INPUT_BUTTON_PINS;

    // Unmask the interrupt
    GPIO_PORTF_IM_R  |= INPUT_BUTTON_PINS;

    /* Lock Port F configuration */
    GPIO_PORTF_LOCK_R = 0;
}


void setup_global_interrupts(void) {
    /* enable interrupt in NVIC and set priority to 5 */
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (3 << 21);     /* set interrupt priority to 5 */
    NVIC_EN0_R |= (1 << 30);  /* enable IRQ30 (D30 of ISER[0]) */

    __enable_irq(); /* global enable IRQs */
}

////////////////////////
// Interrupt Handlers //
////////////////////////


void GPIOF_Handler(void) {
    volatile int readback;

    while (GPIO_PORTF_MIS_R != 0) {
        // NOTE: You cannot have checks on multiple pins in 1 statement or it will not work (i.e. GPIO_PORTF_MIS_R & 0x11)
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


///////////////////
// DAC Converter //
///////////////////
void generate_square_march_waveform_tick(int tick) {
    unsigned char t = (unsigned char) (tick / 60) % 8;
    write_dac_output(1 << t);
}

void generate_sawtooth_waveform_tick(int tick) {
    unsigned char t = (unsigned char) (tick % 256);
    t = t/2;
    write_dac_output(t);
}


void generate_sine_waveform_tick(int tick) {
    // Formula: y = sin(2*pi*(t / T))
    // T = period
    unsigned char t = (unsigned char) (tick % 60);
    float y, y_shifted, y_scaled;

    // Resulting output range: [-1, +1]
    y = sinf(2 * 3.14 * t / 60);

    // Resulting output range: [0, +2]
    y_shifted = y + 1;

    // Resulting output range: [0, +1]
    y_scaled = y_shifted / 2 / 2;

    // Resulting output range: [0, +255]
    write_dac_output((unsigned char) (y_scaled * 255));
}


void write_dac_output(unsigned char input) {
    GPIO_PORTB_DATA_R = input;
}


int main() {
    int tick = 0;

    // Initialize PLL & SysTick
    PLL_Init(SYSDIV2_50_00_Mhz);
    SysTick_Init();

    // Initialize GPIO for Ports F & B
    setup_port_b_pins();
    setup_port_f_pins();

    // Setup interrupts
    setup_global_interrupts();

    while (1) {
        switch (outputMode) {
            case 0:
                write_dac_output(0xFF);
                break;
            case 1:
                generate_sawtooth_waveform_tick(tick);
                break;
            case 2:
                generate_sine_waveform_tick(tick);
                break;
        }

        // NOTE: We make sure to reset the counter to 0 after every 3840 ms
        // - 3840 / 256 = 15 full cycles of sawtooth
        // - 3840 / 60 = 64 full cycles of sine
        tick = (tick + 1) % 3840;
        SysTick_Wait1ms(1);
    }
}

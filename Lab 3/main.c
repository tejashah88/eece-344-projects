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
#include "lib/seg-7/seg-7.h"
#include "lib/pll/PLL.h"
#include "lib/systick/SysTick.h"

// Task pin definitions
#define OUTPUT_LED_SEG_PINS	0xFFu // = 0x80 (PB7) | 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define OUTPUT_DC_PWM_PIN   0x80u // = 0x80 (PA7)

void Setup_Port_A_Pins(void);
void Setup_Port_B_Pins(void);

void SysTick_Wait_200ms(uint32_t delay);
void SysTick_Wait_200us(uint32_t delay);

void Run_Task_1(void);
void Run_Task_2(void);


//////////////////////////
// GPIO Setup functions //
//////////////////////////


void Setup_Port_A_Pins(void) {
    /* Enable Port A clock */
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

	/* Wait until Port A clock is fully initialized */
	while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R0) == 0);

	/* Unlock Port A configuration */
	GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;

	/* Set commit register to only work with pin PA7 */
	GPIO_PORTA_CR_R = OUTPUT_DC_PWM_PIN;

	/* Set PA7 pin as output */
	GPIO_PORTA_DIR_R |= OUTPUT_DC_PWM_PIN;

	/* Disable all analog functionality for PA7 */
	GPIO_PORTA_AMSEL_R &= ~OUTPUT_DC_PWM_PIN;

	/* Enable digital functionality for PA7 */
	GPIO_PORTA_DEN_R |= OUTPUT_DC_PWM_PIN;

	/* Disable all alternate functionality for PA7 */
	GPIO_PORTA_AFSEL_R &= ~OUTPUT_DC_PWM_PIN;

	/* Disable all special functionality for PA7 */
	GPIO_PORTA_PCTL_R &= ~OUTPUT_DC_PWM_PIN;

    /* Lock Port A configuration */
    GPIO_PORTA_LOCK_R = 0;
}


void Setup_Port_B_Pins(void) {
    /* Enable Port B clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    /* Wait until Port B clock is fully initialized */
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R1) == 0);

    /* Unlock Port B configuration */
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;

    /* Set commit register to only work with pins PB0 - PB7 */
    GPIO_PORTB_CR_R = OUTPUT_LED_SEG_PINS;

    /* Set all LED pins (PB0 - PB7) as outputs */
    GPIO_PORTB_DIR_R |= OUTPUT_LED_SEG_PINS;

    /* Disable all analog functionality for PB0 - PB7 */
    GPIO_PORTB_AMSEL_R &= ~OUTPUT_LED_SEG_PINS;

    /* Enable digital functionality for PB0 - PB7 */
    GPIO_PORTB_DEN_R |= OUTPUT_LED_SEG_PINS;

    /* Disable all alternate functionality for PB0 - PB7 */
    GPIO_PORTB_AFSEL_R &= ~OUTPUT_LED_SEG_PINS;

    /* Disable all special functionality for PB0 - PB7 */
    GPIO_PORTB_PCTL_R &= ~OUTPUT_LED_SEG_PINS;

    /* Enable pull-up resistors for PB0 - PB7 */
    GPIO_PORTB_PUR_R |= OUTPUT_LED_SEG_PINS;

    /* Lock Port B configuration */
    GPIO_PORTB_LOCK_R = 0;
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

// Wait for delay x 200 us via SysTick (assuming 50 MHz clock -> 1 cycle = 20 ns)
void SysTick_Wait_200us(uint32_t delay) {
    uint32_t i;
    for (i = 0 ; i < delay; i++) {
        // Wait for duration of 10,000 * 20 ns
        SysTick_Wait(10000);
    }
}

///////////////////////////
// Helper task functions //
///////////////////////////


// Display a countdown from 9 to 0 with a certain delay
void display_seg_7_countdown(void) {
    int32_t i;
    for (i = 9; i >= 0; i--) {
        GPIO_PORTB_DATA_R = SEG_7_PATTERNS[i];
        SysTick_Wait_200ms(5);
    }
}


// Control a DC motor via Pulse-width modulation (PWM) (infinite loop)
void run_dc_motor_pwm(void) {
    // PWM Duty cycle = 80% over 1 ms period
    while (1) {
        // Turn on PA7 for 0.8 ms
        GPIO_PORTA_DATA_R =	0x80;
        SysTick_Wait_200us(1);

        // Turn on PA6 for 0.2 ms
        GPIO_PORTA_DATA_R =	0x00;
        SysTick_Wait_200us(4);
    }
}

//////////////////
// Main program //
//////////////////

/*
 * Sub-task 1: Display a countdown from 9 to 0 with a certain delay
 * Sub-task 2: Control a DC motor via Pulse-width modulation (PWM)
 *
 * I/O Setup
 * - 7-segment LEDs: PB0 - PB7
 * - DC Motor PWM Pin: PA7
 */
int main() {
    // Initialize PLL & SysTick
    PLL_Init(SYSDIV2_50_00_Mhz);
    SysTick_Init();

    // Initialize GPIO pins
    Setup_Port_A_Pins();
    Setup_Port_B_Pins();

    // Run all sub-tasks
    display_seg_7_countdown();
    run_dc_motor_pwm();
}

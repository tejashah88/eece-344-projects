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

#include "tm4c123gh6pm.h"

// Task 1 pin definitions
#define INPUT_BUTTON_PIN	0x20u // = 0x20 (PA5)
#define OUTPUT_LED_PIN		0x40u // = 0x40 (PA6)

// Task 2 pin definitions
#define OUTPUT_TENS_SELECTOR_PIN 0x80u // = 0x80 (PA7)
#define OUTPUT_ONES_SELECTOR_PIN 0x40u // = 0x40 (PA6)
#define OUTPUT_SELECTOR_PINS	 0xC0u // = 0x80 (PA7) | 0x40 (PA6)
#define OUTPUT_LED_SEG_PINS		 0xFFu // = 0x80 (PB7) | 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)

// LED Setup Reference: https://www.geeksforgeeks.org/seven-segment-displays/
#define SEG_7_NUM_0 0x3Fu // = 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_1 0x06u // = 0x04 (PB2) | 0x02 (PB1)
#define SEG_7_NUM_2 0x5Bu // = 0x40 (PB6) | 0x10 (PB4) | 0x08 (PB3) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_3 0x4Fu // = 0x40 (PB6) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_4 0x66u // = 0x40 (PB6) | 0x20 (PB5) | 0x04 (PB2) | 0x02 (PB1)
#define SEG_7_NUM_5 0x6Du // = 0x40 (PB6) | 0x20 (PB5) | 0x08 (PB3) | 0x04 (PB2) | 0x01 (PB0)
#define SEG_7_NUM_6 0x7Du // = 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x01 (PB0)
#define SEG_7_NUM_7 0x07u // = 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_8 0x7Fu // = 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_9 0x6Fu // = 0x40 (PB6) | 0x20 (PB5) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)

const unsigned char SEG_7_PATTERNS[10] = {
    SEG_7_NUM_0,
    SEG_7_NUM_1,
    SEG_7_NUM_2,
    SEG_7_NUM_3,
    SEG_7_NUM_4,
    SEG_7_NUM_5,
    SEG_7_NUM_6,
    SEG_7_NUM_7,
    SEG_7_NUM_8,
    SEG_7_NUM_9,
};

// Function declarations
void Setup_Port_A_Pins(void);
void Setup_Port_B_Pins(void);

void Run_Task_1(void);
void Run_Task_2(void);

void Display_Tens_Digit(unsigned int n);
void Display_Ones_Digit(unsigned int n);

void delayMs(int n);

/////////////////////
// Setup functions //
/////////////////////

void Setup_Port_A_Pins(void) {
    // Enable Port A clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // Wait until Port A clock is fully initialized
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0);

    // Unlock Port A configuration
    GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PA5 - PA7
    GPIO_PORTA_CR_R = (INPUT_BUTTON_PIN | OUTPUT_SELECTOR_PINS);

    // Set output selector pins (PA6 & PA7) as outputs and rest of pins as inputs (implicitly)
    GPIO_PORTA_DIR_R |= OUTPUT_SELECTOR_PINS;

    // Disable analog functionality for PA5 - PA7
    GPIO_PORTA_AMSEL_R &= ~(INPUT_BUTTON_PIN | OUTPUT_SELECTOR_PINS);

    // Enable digital functionality for PA5 - PA7
    GPIO_PORTA_DEN_R |= (INPUT_BUTTON_PIN | OUTPUT_SELECTOR_PINS);

    // Disable all alternate functionality for PA5 - PA7
    GPIO_PORTA_AFSEL_R &= ~(INPUT_BUTTON_PIN | OUTPUT_SELECTOR_PINS);

    // Disable all special functionality for PA5 - PA7
    GPIO_PORTA_PCTL_R &= ~(INPUT_BUTTON_PIN | OUTPUT_SELECTOR_PINS);

    // Enable pull-up resistors for input button pin (PA5)
    GPIO_PORTA_PUR_R |= INPUT_BUTTON_PIN;

    // Lock Port A configuration
    GPIO_PORTA_LOCK_R = 0;
}


void Setup_Port_B_Pins(void) {
    // Enable Port B clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Wait until Port B clock is fully initialized
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);

    // Unlock Port B configuration
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PB0 - PB7
    GPIO_PORTB_CR_R = OUTPUT_LED_SEG_PINS;

    // Set all LED pins (PB0 - PB7) as outputs
    GPIO_PORTB_DIR_R |= OUTPUT_LED_SEG_PINS;

    // Disable all analog functionality for PB0 - PB7
    GPIO_PORTB_AMSEL_R &= ~OUTPUT_LED_SEG_PINS;

    // Enable digital functionality for PB0 - PB7
    GPIO_PORTB_DEN_R |= OUTPUT_LED_SEG_PINS;

    // Disable all alternate functionality for PB0 - PB7
    GPIO_PORTB_AFSEL_R &= ~OUTPUT_LED_SEG_PINS;

    // Disable all special functionality for PB0 - PB7
    GPIO_PORTB_PCTL_R &= ~OUTPUT_LED_SEG_PINS;

    // Enable pull-up resistors for PB0 - PB7
    GPIO_PORTB_PUR_R |= OUTPUT_LED_SEG_PINS;

    // Lock Port B configuration
    GPIO_PORTB_LOCK_R = 0;
}

////////////////////////
// Lab Task functions //
////////////////////////

/*
 * Task 1: Detecting push button. If pressed, turn on the LED. Else, turn off the LED.
 *
 * I/O Setup
* - Push Button: PA5
* - Standalone LED: PA6
*/
void Run_Task_1(void) {

    for (;;) {
        // Check if input button is pressed or not via PA5
        if ((GPIO_PORTA_DATA_R & INPUT_BUTTON_PIN) == 1) { // = 0x20 (PA5)
            // Turn on output LED via PA6
            GPIO_PORTA_DATA_R |= OUTPUT_LED_PIN;	// = 0x40 (PA6)
        } else {
            // Turn off output LED via PA6
            GPIO_PORTA_DATA_R &= ~OUTPUT_LED_PIN;	// = 0x40 (PA6)
        }
    }
}


/*
 * Task 2: Simultaneously display two numbers on two 7-segments.
 *
 * I/O Setup
 * - Digit Selectors: PA6 & PA7
 * - 7-segment LEDs: PB0 - PB7
 */
void Run_Task_2(void) {
    unsigned int n = 0, counter = 0;
    unsigned int tens = 0, ones = 0;

    for (;;) {
        // Trigger an increment to the display number (n) roughly every 500 ms (10 ms per iteration * 50 counts)
        if (counter >= 50) {
            counter = 0;
            n = (n + 1) % 100;

            // Decompose number into individual digits
            tens = n / 10;
            ones = n % 10;
        }

        // Display tens digit
        Display_Tens_Digit(tens);

        // Delay for 5 ms will result in 10 ms per loop, making human eyes not detect on/off of thedisplay
        delayMs(5);

        // Display ones digit
        Display_Ones_Digit(ones);

        // Delay 5 ms will result in 10 ms per loop
        delayMs(5);

        counter++;
    }
}

///////////////////////
// Display functions //
///////////////////////

void Display_Tens_Digit(unsigned int n) {
    // Drive pattern of first number on 7-segment LEDs
    GPIO_PORTB_DATA_R = SEG_7_PATTERNS[n];

    // Enable left digit via PA7
    GPIO_PORTA_DATA_R &= ~OUTPUT_ONES_SELECTOR_PIN;	// = 0x40 (PA6)

    // Disable right digit via PA6
    GPIO_PORTA_DATA_R |= OUTPUT_TENS_SELECTOR_PIN;	// = 0x80 (PA7)
}


void Display_Ones_Digit(unsigned int n) {
    // Drive pattern of second number on 7-segment LEDs
    GPIO_PORTB_DATA_R = SEG_7_PATTERNS[n];

    // Disable left digit via PA7
    GPIO_PORTA_DATA_R |= OUTPUT_ONES_SELECTOR_PIN;	// = 0x40 (PD6)

    // Enable right digit via PA6
    GPIO_PORTA_DATA_R &= ~OUTPUT_TENS_SELECTOR_PIN;	// = 0x80 (PA7)
}

///////////////////////
// Utility functions //
///////////////////////

// Delay by 'n' milliseconds (16 MHz CPU clock)
void delayMs(int n)
{
    int i, j;
    for (i = 0 ; i < n; i++)
        for (j = 0; j < 3180; j++)
            {} // do nothing for 1 ms
}

//////////////////
// Main program //
//////////////////

// Used for defining which task from the lab to run. Change number and recompile as necessary.
#define TASK_NUM 2

// Main loop
int main(void)
{
    // Setup phase
    Setup_Port_A_Pins();
    Setup_Port_B_Pins();

    // Running task phase
    // NOTE: Change TASK_NUM above based on which lab task to run and recompile
#if TASK_NUM == 1
    Run_Task_1();
#elif TASK_NUM == 2
    Run_Task_2();
#else
    return 0;
#endif
}

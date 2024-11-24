/*
 * Reference material:
 * - MCU Datasheet: https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf
 * - Suplimental info: https://web2.qatar.cmu.edu/cs/15348/lectures/Lecture03.pdf
 * - LCD Datasheet: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 *
 * Useful info:
 * - Pin to hex: (1 << pinN)
 * - Bit manipulation:
 *   - Bit set: (arg |= (1 << pinN))
 *   - Bit clear: (arg &= ~(1 << pinN))
 *   - Bit toggle: (arg ^= (1 << pinN))
 *   - Bit test: (arg & (1 << pinN) == 1)
 */

// Include the Device header
#include "mcu/tm4c123gh6pm.h"
#include "mcu/mcu_utils.h"
#include "lcd/lcd_driver.h"
#include "keypad/keypad_driver.h"

void Run_Task_1(void);
void Run_Task_2(void);

/* Used for defining which task from the lab to run. Change number and recompile as necessary. */
#define TASK_NUM 1

int main() {
    /* Running task phase */
    // NOTE: Change TASK_NUM above based on which lab task to run and recompile
#if TASK_NUM == 1
    Run_Task_1();
#elif TASK_NUM == 2
    Run_Task_2();
#else
    return 0;
#endif
}

////////////////////////
// Lab Task functions //
////////////////////////


/*
 * Task 1: Display characters from string onto LCD screen.
 *
 * I/O Setup
 * - LCD Control Pins:
 *  - RS pin: PB0
 *  - RW pin: PB1
 *  - EN pin: PB2
 * - LCD Data Pins:
 *  - Data 0 pin: PB4
 *  - Data 1 pin: PB5
 *  - Data 2 pin: PB6
 *  - Data 3 pin: PB7
 */
void Run_Task_1(void) {
    int i;
    char ch;
    const unsigned char DISPLAY_STRING[] = "HELLO WORLD";

    // Call the LCD initialization function
    LCD_4Bits_Init();

    // Clear the LCD screen
    LCD4Bits_Cmd(LCD_CLEAR_DISPLAY);

    // Set the cursor to the beginning of the first line
    LCD4Bits_Cmd(LCD_SET_DDRAM_ADDR + LCD_LINE1_START);

    // Allow some delay, so the slow LCD will catch up with the fast MCU.
    Delay_Milli(500);

    i = 0;
    while (DISPLAY_STRING[i] != '\0') {
        ch = DISPLAY_STRING[i];

        // Test the LCD by sending a character.
        LCD4Bits_Data(ch);

        // Allow some delay, so the slow LCD will catch up with the fast MCU.
        Delay_Milli(500);

        i++;
    }
}


/*
 * Task 2: Read inputs from keypad and display onto LCD screen.
 *
 * I/O Setup
 * - LCD Control Pins:
 *  - RS pin: PB0
 *  - RW pin: PB1
 *  - EN pin: PB2
 * - LCD Data Pins:
 *  - Data 0 pin: PB4
 *  - Data 1 pin: PB5
 *  - Data 2 pin: PB6
 *  - Data 3 pin: PB7
 * - Keypad row pins:
 *  - Row 1 pin: PE0
 *  - Row 2 pin: PE1
 *  - Row 3 pin: PE2
 *  - Row 4 pin: PE3
 * - Keypad columns pins:
 *  - Column 1 pin: PC4
 *  - Column 2 pin: PC5
 *  - Column 3 pin: PC6
 *  - Column 4 pin: PC7
 */
void Run_Task_2(void) {
    unsigned char key;
    int key_count = 0;

    // Initialize keypad & LCD
    Keypad_Init();
    LCD_4Bits_Init();

    // Clear the LCD screen
    LCD4Bits_Cmd(LCD_CLEAR_DISPLAY);

    // Set the cursor to the beginning of the first line
    LCD4Bits_Cmd(LCD_SET_DDRAM_ADDR + LCD_LINE1_START);

    // Allow some delay, so the slow LCD will catch up with the fast MCU.
    Delay_Milli(500);

    for (;;) {
        key = getKey();	// call the function to get a key input
        Delay_Milli(800);	// give the mechanic key some time to debounce.

        if (key != 0) {
            key_count++;

            if (key_count >= 16) {
                key_count = 0;	// It's better to clear the LCD screen too.

                // Clear the LCD screen
                LCD4Bits_Cmd(LCD_CLEAR_DISPLAY);

                // Set the cursor to the beginning of the first line
                LCD4Bits_Cmd(LCD_SET_DDRAM_ADDR + LCD_LINE1_START);
            }

            // Display the key input on your LCD.
            LCD4Bits_Data(key);
            Delay_Milli(500);
        }
    }
}

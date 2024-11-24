#include "mcu/tm4c123gh6pm.h"
#include "mcu/mcu_utils.h"
#include "keypad_driver.h"


const unsigned char KEYMAP[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};


void Keypad_Init(void) {
    Setup_Keypad_GPIO_Row_Pins();
    Setup_Keypad_GPIO_Column_Pins();
}


void Setup_Keypad_GPIO_Column_Pins(void) {
    /* Enable Port C clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;

    /* Wait until Port C clock is fully initialized */
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R2) == 0);

    /* Unlock Port C configuration */
    GPIO_PORTC_LOCK_R = GPIO_LOCK_KEY;

    /* Set commit register to only work with pins PC4 - PC7 */
    GPIO_PORTC_CR_R = KEYPAD_ALL_COLS;

    /* Set all LED pins (PC4 - PC7) as inputs */
    GPIO_PORTC_DIR_R |= ~KEYPAD_ALL_COLS;

    /* Disable all analog functionality for PC4 - PC7 */
    GPIO_PORTC_AMSEL_R &= ~KEYPAD_ALL_COLS;

    /* Enable digital functionality for PC4 - PC7 */
    GPIO_PORTC_DEN_R |= KEYPAD_ALL_COLS;

    /* Disable all alternate functionality for PC4 - PC7 */
    GPIO_PORTC_AFSEL_R &= ~KEYPAD_ALL_COLS;

    /* Disable all special functionality for PC4 - PC7 */
    GPIO_PORTC_PCTL_R &= ~KEYPAD_ALL_COLS;

    /* Enable pull-up resistors for PC4 - PC7 */
    GPIO_PORTC_PUR_R |= KEYPAD_ALL_COLS;

    /* Lock Port C configuration */
    GPIO_PORTC_LOCK_R = 0;
}


void Setup_Keypad_GPIO_Row_Pins(void) {
    /* Enable Port E clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;

    /* Wait until Port E clock is fully initialized */
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R4) == 0);

    /* Unlock Port E configuration */
    GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;

    /* Set commit register to only work with pins PE0 - PE3 */
    GPIO_PORTE_CR_R = KEYPAD_ALL_ROWS;

    /* Set all LED pins (PE0 - PE3) as outputs */
    GPIO_PORTE_DIR_R |= KEYPAD_ALL_ROWS;

    /* Disable all analog functionality for PE0 - PE3 */
    GPIO_PORTE_AMSEL_R &= ~KEYPAD_ALL_ROWS;

    /* Enable digital functionality for PE0 - PE3 */
    GPIO_PORTE_DEN_R |= KEYPAD_ALL_ROWS;

    /* Disable all alternate functionality for PE0 - PE3 */
    GPIO_PORTE_AFSEL_R &= ~KEYPAD_ALL_ROWS;

    /* Disable all special functionality for PE0 - PE3 */
    GPIO_PORTE_PCTL_R &= ~KEYPAD_ALL_ROWS;

    /* Enable open-drain mode for PE0 - PE3 */
    GPIO_PORTE_ODR_R |= KEYPAD_ALL_ROWS;

    /* Lock Port E configuration */
    GPIO_PORTE_LOCK_R = 0;
}


unsigned char getKey(void) {
    unsigned char k_row, k_col;
    unsigned char colPins;

    // Set all rows to GND
    GPIO_PORTE_DATA_R = 0x0F;

    // Read the key inputs on PC4 - PC7
    k_col = (GPIO_PORTC_DATA_R & 0xF0);

    // If no key is pressed, return early
    if (k_col == 0xF0) {
        return 0;
    }

    while (1) {
        for (k_row = 0; k_row <= 3; k_row++) {
            // Set k_row-th to GND
            GPIO_PORTE_DATA_R = (0x0F & ~(1 << k_row));

            // Delay 10 us to handle debounce from mechanical key
            Delay_Micro(10);

            // Read each column from PC4 to PC7
            colPins = (GPIO_PORTC_DATA_R & 0xF0) >> 4;
            for (k_col = 0; k_col <= 3; k_col++) {
                // Check that the k_col-th column has a key pressed
                if (colPins == (0x0F & ~(1 << k_col))) {
                    // Return the corresponding character from the row and column key combo pressed
                    return KEYMAP[k_row][k_col];
                }
            }
        }
    }
}

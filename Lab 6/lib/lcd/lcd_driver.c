#include <stdint.h>

#include "mcu/tm4c123gh6pm.h"
#include "timing_util/timing.h"
#include "lcd_driver.h"


/*
 * I/O Setup
 * - LCD Control Pins:
 *  - RS pin: PB0
 *  - RW pin: PB1
 *  - EN pin: PB2
 * - LCD Data Pins (4-bit mode):
 *  - Data 0 pin: PB4
 *  - Data 1 pin: PB5
 *  - Data 2 pin: PB6
 *  - Data 3 pin: PB7
 */


void LCD_4Bits_Init(void) {
    // Setup LCD GPIO Pins
    Setup_LCD_GPIO_Pins();

    // Sets cursor to beginning of display and unshifts display if set before
    LCD_4Bits_Cmd(LCD_RETURN_HOME); // 0x02

    // Sets LCD to operate in 4-bit mode, using 2 lines and 5 x 8 fonts
    // NOTE: Function set command MUST be run first before any other LCD commands
    LCD_4Bits_Cmd(LCD_FUNCTION_SET | LCD_4_BIT_MODE | LCD_2_LINE_MODE | LCD_5x8_FONT_MODE); // 0x28

    // Set cursor to increment automatically after each character insert
    LCD_4Bits_Cmd(LCD_ENTRY_MODE_SET | LCD_ENTRY_CURSOR_AUTO_MOVE_RIGHT | LCD_ENTRY_NO_SHIFT_DISPLAY); // 0x06

    // Turn on the display & cursor and set cursor to blinking
    LCD_4Bits_Cmd(LCD_DISPLAY_CTRL | LCD_DISPLAY_FULL_ON | LCD_DISPLAY_CURSOR_ON | LCD_DISPLAY_BLINKING_CURSOR_ON); // 0x0F

    // Clear the LCD display
    LCD_4Bits_Cmd(LCD_CLEAR_DISPLAY); // 0x01
}


void Setup_LCD_GPIO_Pins(void) {
    // Enable Port B clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Wait until Port B clock is fully initialized
    while ((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R1) == 0);

    // Unlock Port B configuration
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PB0 - PB2 & PB4 - PB7
    GPIO_PORTB_CR_R = LCD_ALL_PINS;

    // Set all LED pins (PB0 - PB2 & PB4 - PB7) as outputs
    GPIO_PORTB_DIR_R |= LCD_ALL_PINS;

    // Disable all analog functionality for PB0 - PB2 & PB4 - PB7
    GPIO_PORTB_AMSEL_R &= ~LCD_ALL_PINS;

    // Enable digital functionality for PB0 - PB2 & PB4 - PB7
    GPIO_PORTB_DEN_R |= LCD_ALL_PINS;

    // Disable all alternate functionality for PB0 - PB2 & PB4 - PB7
    GPIO_PORTB_AFSEL_R &= ~LCD_ALL_PINS;

    // Disable all special functionality for PB0 - PB2 & PB4 - PB7
    GPIO_PORTB_PCTL_R &= ~LCD_ALL_PINS;

    // Lock Port B configuration
    GPIO_PORTB_LOCK_R = 0;
}


void LCD_Write4Bits(uint8_t data, uint8_t control) {
    // Compose the data packet
    //   PB0 - PB2 -> control
    //   PB4 - PB7 -> data
    uint8_t dataPacket = ((data & 0xF0) | (control & 0x0F));

    // Send both data and the control signals to LCD through the GPIODATA register. Secure the sent signals by EN.
    GPIO_PORTB_DATA_R = (dataPacket | LCD_EN_ENABLE_MODE);

    // Allow some delay to let sent data be processed by LCD
    SysTick_Wait_1us(3);

    // Set back to the data without EN.
    GPIO_PORTB_DATA_R ^= LCD_EN_ENABLE_MODE;
}


void LCD_Write8Bits_4BitMode(uint8_t data, uint8_t control) {
    // Send upper nibble of data packet
    LCD_Write4Bits(data & 0xF0, control);

    // Send lower nibble of data packet
    LCD_Write4Bits((data & 0x0F) << 4, control);
}


void LCD_4Bits_Cmd(uint8_t command) {
    // Send command packet in 4-bit mode
    LCD_Write8Bits_4BitMode(command, (LCD_RW_WRITE_MODE | LCD_RS_COMMAND_MODE));

    // Allow some delay as specified from LCD datasheet
    if (command < 4)
        SysTick_Wait_1ms(2);
    else
        SysTick_Wait_1us(37);
}


void LCD_4Bits_Data(uint8_t data) {
    // Send data packet in 4-bit mode
    LCD_Write8Bits_4BitMode(data, (LCD_RW_WRITE_MODE | LCD_RS_DATA_MODE));

    // Allow some delay as specified from LCD datasheet
    SysTick_Wait_1us(37);
}


void LCD_4Bits_OutputString(char* str) {
    char ch;
    int i = 0;

    while (str[i] != '\0' && i < 16) {
        ch = str[i];

        // Send the current character to the LCD
        LCD_4Bits_Data(ch);

        // Allow some delay to let the LCD display the character
        SysTick_Wait_500ms(1);

        i++;
    }
}

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
#include <stdio.h>

#include "lib/mcu/tm4c123gh6pm.h"
#include "lib/pll/PLL.h"
#include "lib/systick/SysTick.h"
#include "lib/timing_util/timing.h"
#include "lib/adc/adc_temp.h"
#include "lib/lcd/lcd_driver.h"


// Voltage Reference values
#define VREF_POS 3.3
#define VREF_NEG 3.3


int main() {
    // Initialize PLL & SysTick
    PLL_Init(SYSDIV2_50_00_Mhz);
    SysTick_Init();

    // Initialize ADC module and internal temperature sensor (PE3)
    ADC_Temp_Sensor_Init();

    // Initialize the LCD
    LCD_4Bits_Init();

    // Clear the LCD screen
    LCD_4Bits_Cmd(LCD_CLEAR_DISPLAY);

    // Set the cursor to the beginning of the first line
    LCD_4Bits_Cmd(LCD_SET_DDRAM_ADDR + LCD_LINE1_START);

    // Allow some delay to let the LCD display the character
    SysTick_Wait_500ms(1);

    // Initialize GPIO for Port E (internal temperature sensor)
    Setup_Port_E_Pins();

    while (1) {
        // Fetch the latest temperature reading (as raw digital voltage) from PE3
        uint32_t tempRaw = Get_ADC_Temp_Reading();

        // Convert temperature from raw voltage reading to Celsius
        float tempCelsius = Convert_Temp_Voltage_Celsius(VREF_POS, VREF_NEG, tempRaw);

        // Prepare the display string for the LCD
        char lineBuffer[16];
        //   NOTE: This format allows for negative temperatures and up
        //   to 2 decimals within a single LCD line (max 16 characters).
        sprintf(lineBuffer, "Temp (C): %.2f.", tempCelsius);

        // Display the string to the LCD (includes automatic handling of delays)
        LCD_4Bits_OutputString(lineBuffer);
    }
}

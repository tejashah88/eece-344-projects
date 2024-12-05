#include <stdint.h>
#include "mcu/tm4c123gh6pm.h"
#include "PLL.h"

// Configure the system to get its clock from the PLL
// Sources:
// - Section 5.3 in MCU datasheet (https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
// - RCC2 Register - pg. 260 of MCU datasheet (above)
// - https://alhabish.github.io/embedded-course/2018/06/20/pll.html
int PLL_Init(uint8_t sysdiv2_divisor) {
    // Step 1: Configure the system to use RCC2 for advanced features
    // such as 400 MHz PLL and non-integer System Clock Divisor.
    SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;

    // Step 2: Bypass PLL & disable System Clock Divider during initialization
    SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
    SYSCTL_RCC2_R &= ~SYSCTL_RCC_USESYSDIV;

    // Step 3: Select the crystal value and oscillator source
    SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;      // Clear the XTAL field
    SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;   // Configure for 16 MHz crystal
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M; // Clear oscillator source field
    SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO; // Configure for main oscillator source

    // Step 4: Activate PLL by clearing the PWRDN (Power Down) bit
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;

    // Step 5: Set the desired system divider and the system divider least significant bit
    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;     // use 400 MHz PLL

    // Clear system clock divider field + LSB bit (bits 28-23 + 22 in RCC2 - pg. 260)
    SYSCTL_RCC2_R &= ~(SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB);

    // Set the system clock frequency based on the following formula: 400 Mhz / (SYSDIV2 + 1)
    // See Table 5-6 on pg. 224 of the MCU datasheet for example frequencies and limitations
    SYSCTL_RCC2_R += (sysdiv2_divisor << 22);

    // Step 6: Wait for PLL to lock by polling PLLLRIS (PLL Lock Raw Interrupt Status)
    while ((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0);

    // Step 7: Re-enable use of PLL & System Clock Divider
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
    SYSCTL_RCC2_R |= SYSCTL_RCC_USESYSDIV;

    return 0;
}

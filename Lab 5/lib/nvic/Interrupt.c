#include <stdint.h>

#include "mcu/tm4c123gh6pm.h"
#include "Interrupt.h"

// Array for holding list of interrupt enable registers
volatile unsigned long *const NVIC_ENABLE_REG[5] = {
    &NVIC_EN0_R,
    &NVIC_EN1_R,
    &NVIC_EN2_R,
    &NVIC_EN3_R,
    &NVIC_EN4_R,
};


// Array for holding list of interrupt priority registers
volatile unsigned long *const NVIC_PRIORITY_REG[35] = {
    &NVIC_PRI0_R,
    &NVIC_PRI1_R,
    &NVIC_PRI2_R,
    &NVIC_PRI3_R,
    &NVIC_PRI4_R,
    &NVIC_PRI5_R,
    &NVIC_PRI6_R,
    &NVIC_PRI7_R,
    &NVIC_PRI8_R,
    &NVIC_PRI9_R,
    &NVIC_PRI10_R,
    &NVIC_PRI11_R,
    &NVIC_PRI12_R,
    &NVIC_PRI13_R,
    &NVIC_PRI14_R,
    &NVIC_PRI15_R,
    &NVIC_PRI16_R,
    &NVIC_PRI17_R,
    &NVIC_PRI18_R,
    &NVIC_PRI19_R,
    &NVIC_PRI20_R,
    &NVIC_PRI21_R,
    &NVIC_PRI22_R,
    &NVIC_PRI23_R,
    &NVIC_PRI24_R,
    &NVIC_PRI25_R,
    &NVIC_PRI26_R,
    &NVIC_PRI27_R,
    &NVIC_PRI28_R,
    &NVIC_PRI29_R,
    &NVIC_PRI30_R,
    &NVIC_PRI31_R,
    &NVIC_PRI32_R,
    &NVIC_PRI33_R,
    &NVIC_PRI34_R,
};

// Sources:
// * Register mapping: pg. 134 - 137 of datasheet (https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
// * Priority bit field locations: pg. 152 of datasheet (https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
// * Formula calculations: https://www.airsupplylab.com/ti-tiva-series/tiva_lesson-14-interrupt.html


// Enable the specified interrupt (number)
//   This works by finding the specified "interrupt enable register" and setting
//   the specific bit, with a interrupt number limit between 0 and 138.
void NVIC_EnableIRQn(int IRQn) {
    // Enable register index: i = n >> 5 (same as i = n // 32)
    uint32_t idxEnableRegister = IRQn >> 5;

    // Bit index: bit field = 1 << b, where b = 0 to 31 (same as b = n % 32)
    uint32_t bitIndex = IRQn & 0x1F;

    // Enable the target interrupt via the interrupt number within
    *NVIC_ENABLE_REG[idxEnableRegister] |= (1 << bitIndex);
}


// Set the priority for the specified interrupt (number)
//   This works by finding the "interrupt priority register" and setting the priority
//   via a 3-bit field section, with the lowest priority being 0 and highest being 7.
void NVIC_SetPriorityIRQn(int IRQn, int priority) {
    // Priority register index: i = n >> 2 (same as i = n // 4)
    uint32_t idxPriorityRegister = IRQn >> 2;

    // Register 3-bit section: s = 5 + 8 * p, where p = 0 to 3 (same as p = n % 4)
    // * Section 0 = bits[ 7: 5] (5 + 8 * 0 => 5)
    // * Section 1 = bits[15:13] (5 + 8 * 1 => 13)
    // * Section 2 = bits[23:21] (5 + 8 * 2 => 21)
    // * Section 3 = bits[31:29] (5 + 8 * 3 => 29)
    uint32_t p = IRQn & 0x3U;

    // Clear the previously set priority in the 3-bit section for the target interrupt
    *NVIC_PRIORITY_REG[idxPriorityRegister] &= ~(0xFF << (8 * p));

    // Save the new priority in the same 3-bit section for the target interrupt
    *NVIC_PRIORITY_REG[idxPriorityRegister] |= (priority << (5 + 8 * p));
}

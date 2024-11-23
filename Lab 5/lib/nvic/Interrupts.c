#include <stdint.h>

#include "mcu/tm4c123gh6pm.h"
#include "Interrupts.h"


static volatile unsigned long *const NVIC_ENABLE_REGS[5] = {
    &NVIC_EN0_R,
    &NVIC_EN1_R,
    &NVIC_EN2_R,
    &NVIC_EN3_R,
    &NVIC_EN4_R,
};


static volatile unsigned long *const NVIC_PRIORITY_REGS[35] = {
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


// Limit: 0 and 138
void NVIC_EnableIRQ(int IRQn) {
    // Enable index = IRQn >> 5
    // Bit set = 1 << b, where b = 0 to 31
    *NVIC_ENABLE_REGS[IRQn >> 5] |= (1 << (IRQn & 0x1F));
}


void NVIC_SetPriority(int IRQn, int priority) {
    /* enable interrupt in NVIC and set priority to 5 */
    *NVIC_PRIORITY_REGS[priority] &= 0xFF00FFFF;
    *NVIC_PRIORITY_REGS[priority] |= (3 << 21);

    NVIC_EN0_R |= (1 << 30);  /* enable IRQ30 (D30 of ISER[0]) */


    // NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (3 << 21);     /* set interrupt priority to 5 */

    __enable_irq(); /* global enable IRQs */
}

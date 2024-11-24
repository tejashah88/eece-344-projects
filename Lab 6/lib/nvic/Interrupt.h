#ifndef MCU_NVIC_INTERRUPT
#define MCU_NVIC_INTERRUPT

// README/HACK: The following arrays is bad practice if deploying to production, but
// for EECE classes it's okay. Never recommended to hold arrays of register addresses,
// should use "TM4C123.h" in place of given header file for memory-mapped style.

// Array for holding list of interrupt enable registers
// NOTE: If implementing for a new board, double check the datasheet for size specification
extern volatile unsigned long *const NVIC_ENABLE_REG[];

// Array for holding list of interrupt priority registers
// NOTE: If implementing for a new board, double check the datasheet for size specification
extern volatile unsigned long *const NVIC_PRIORITY_REG[];


// Enable the specified interrupt (number)
extern void NVIC_EnableIRQn(int IRQn);

// Set the priority for the specified interrupt (number)
extern void NVIC_SetPriorityIRQn(int IRQn, int priority);


#endif /* MCU_NVIC_INTERRUPT */

#ifndef MCU_SYSTICK
#define MCU_SYSTICK

// Configure SysTick with busy wait running at initialized bus clock (from PLL)
extern void SysTick_Init(void);

// Unit time delay based on system clock
// NOTE: It's better to construct higher level functions with greater delays to reduce function calling overhead
// Delay parameter units is 1 per cycle time (typically in nanoseconds)
extern void SysTick_Wait(uint32_t delay);


#endif /* MCU_SYSTICK */

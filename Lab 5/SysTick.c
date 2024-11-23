#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}

// Time delay using busy wait.
// The delay parameter is in units of the core clock.
void SysTick_Wait(uint32_t delay){
  NVIC_ST_RELOAD_R = delay - 1;	// number of counts to wait;
	NVIC_ST_CURRENT_R = 0;	// clear previous setting;
	while((NVIC_ST_CTRL_R & 0x00010000) == 0) {}		// wait for the count flag
}

void SysTick_Wait1ms(uint32_t delay){
  uint32_t i;	// local variable to control the for loop
  for(i = 0 ; i < delay; i++){
			SysTick_Wait(50000);	// wait 1ms (assumes 50 MHz clock) one cycle is 20 ns
  }
}


#include "mcu_utils.h"

///////////////////////
// Utility functions //
///////////////////////

/* Delay by 'n' microseconds (16 MHz CPU clock) */
void delay_micro(int n) {
	int i, j;
	for (i=0; i<n; i++) {
		for(j=0;j<3;j++)
			{} /* do nothing for 1 us */
	}
}

/* Delay by 'n' milliseconds (16 MHz CPU clock) */
void delay_milli(int n) {
	int i, j;
	for (i=0; i<n; i++) {
		for(j=0;j<3180;j++)
			{} /* do nothing for 1 ms */
	}
}

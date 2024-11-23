#ifndef MCU_PLL
#define MCU_PLL

// Common SYSDIV2 divisors for initializing PLL based on desired frequency
// Source: See Table 5-6 on pg. 224 of the MCU datasheet for more example frequencies and limitations
// Formula used based on frequency (assuming max PLL clock is 400 MHz):
//   System clock frequency = 400 MHz / (SYSDIV2 + 1)

#define SYSDIV2_80_00_Mhz 4
#define SYSDIV2_66_67_Mhz 5
#define SYSDIV2_50_00_Mhz 7
#define SYSDIV2_44_44_Mhz 8
#define SYSDIV2_40_00_Mhz 9
#define SYSDIV2_33_33_Mhz 11
#define SYSDIV2_26_67_Mhz 14
#define SYSDIV2_25_00_Mhz 15
#define SYSDIV2_22_22_Mhz 17
#define SYSDIV2_20_00_Mhz 19
#define SYSDIV2_16_67_Mhz 23
#define SYSDIV2_16_00_Mhz 24
#define SYSDIV2_13_33_Mhz 29
#define SYSDIV2_12_50_Mhz 31
#define SYSDIV2_11_11_Mhz 35
#define SYSDIV2_10_00_Mhz 39
#define SYSDIV2_8_888_Mhz 44
#define SYSDIV2_8_333_Mhz 47
#define SYSDIV2_8_000_Mhz 49
#define SYSDIV2_6_666_Mhz 59
#define SYSDIV2_6_250_Mhz 59
#define SYSDIV2_5_556_Mhz 71
#define SYSDIV2_5_333_Mhz 74
#define SYSDIV2_5_000_Mhz 79
#define SYSDIV2_4_444_Mhz 89
#define SYSDIV2_4_000_Mhz 99
#define SYSDIV2_3_333_Mhz 119
#define SYSDIV2_3_200_Mhz 124
#define SYSDIV2_3_125_Mhz 127


// Configure the system to get its clock from the PLL
extern int PLL_Init(uint8_t sysdiv2_divisor);


#endif /* MCU_PLL */

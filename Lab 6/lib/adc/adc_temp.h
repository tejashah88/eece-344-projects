#ifndef ADC_TEMP
#define ADC_TEMP

#include <stdint.h>

// Pin definitions
#define INTERNAL_TEMP_SENSOR_PIN    0x08u // = 0x08 (PE3)


// Setup function definitions
extern void Setup_Port_E_Pins(void);
extern void ADC0_Module_Init(void);
extern void ADC_Temp_Sensor_Init(void);

// Temperature reading function definitions
extern uint32_t Get_ADC_Temp_Reading(void);
extern float Convert_Temp_Voltage_Celsius(float vRefPos, float vRefNeg, uint32_t adcReading);


#endif /* ADC_TEMP */

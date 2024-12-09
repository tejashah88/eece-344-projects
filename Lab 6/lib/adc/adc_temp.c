#include <stdint.h>

#include "mcu/tm4c123gh6pm.h"
#include "adc_temp.h"


// Setup GPIO Port E and pin PE3, using internal temperature sensor
void Setup_Port_E_Pins(void) {
    // Enable Port E clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;

    // Wait until Port E clock is fully initialized
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);

    // Unlock Port E configuration
    GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;

    // Set commit register to only work with pins PE3
    GPIO_PORTE_CR_R = INTERNAL_TEMP_SENSOR_PIN;

    // Set input buttons pins (PE3) as inputs
    GPIO_PORTE_DIR_R &= ~INTERNAL_TEMP_SENSOR_PIN;

    // Enable analog functionality for PE3
    GPIO_PORTE_AMSEL_R |= INTERNAL_TEMP_SENSOR_PIN;

    // Disable digital functionality for PE3
    GPIO_PORTE_DEN_R &= ~INTERNAL_TEMP_SENSOR_PIN;

    // Enable all alternate functionality for PE3
    GPIO_PORTE_AFSEL_R |= INTERNAL_TEMP_SENSOR_PIN;

    // Disable all special functionality for PE3
    GPIO_PORTE_PCTL_R &= ~INTERNAL_TEMP_SENSOR_PIN;
}


// Setup ADC 0 module, using internal temperature sensor
void ADC0_Module_Init(void) {
    // Enable clock for ADC module
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;

    // Wait until ADC 0 clock is fully initialized
    while ((SYSCTL_PRADC_R & SYSCTL_PRADC_R0) == 0);

    // Disable Sample Sequence 3 (SS3) during configuration
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3;

    // Set the sampling trigger to be software-triggered (manual)
    ADC0_EMUX_R &= ~ADC_EMUX_EM3_M;

    // Clear the SS3 MUX selector
    ADC0_SSMUX3_R &= ~ADC_SSMUX3_MUX0_M;

    // Select the Analog Input 0 (Ain0) channel on PE3
    ADC0_SSMUX3_R += 0;

    // Take only 1 sample at a time from the internal temperature sensor
    // NOTE: The internal temperature sensor does not have a differential option
    ADC0_SSCTL3_R = (ADC_SSCTL3_TS0 | ADC_SSCTL3_IE0 | ADC_SSCTL3_END0);

    // Re-enable Sample Sequence 3 (SS3)
    // NOTE: When querying for the ADC value from PE3, SSE3 will take 1 sample
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;
}


// Fully initialize ADC module and internal temperature sensor
void ADC_Temp_Sensor_Init(void) {
    // Setup GPIO Port E and pin PE3, using internal temperature sensor
    Setup_Port_E_Pins();

    // Setup ADC0 module, using internal temperature sensor
    ADC0_Module_Init();
}


// Fetch raw ADC value of internal temperature sensor
//   Output range (unsigned int): [0, 4095]
uint32_t Get_ADC_Temp_Reading(void) {
    uint32_t adcValue;

    // Enable SS3 conversion or start sampling data from Ain0
    ADC0_PSSI_R = ADC_PSSI_SS3;

    // Poll the Raw Interrupt Status (RIS) until the ADC sample conversion has completed
    while ((ADC0_RIS_R & ADC_RIS_INR3) == 0);

    // Read the ADC coversion result from the SS3 FIFO queue
    adcValue = ADC0_SSFIFO3_R;

    // Clear the conversion flag to allow sampling from ADC0 again
    ADC0_ISC_R |= ADC_ISC_IN3;

    return adcValue;
}


// Convert raw ADC value of internal temperature sensor to Celsius
//   Input range (unsigned int): [0, 4095]
//   Output range (float): [-40 C, +85 C]
float Convert_Temp_Voltage_Celsius(float vRefPos, float vRefNeg, uint32_t adcReading) {
    // Formula source: pg. 813 of datasheet (https://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf)
    float tempCelsius = 147.5f - ((75 * (vRefPos - vRefNeg) * (float) adcReading) / 4096);
    return tempCelsius;
}

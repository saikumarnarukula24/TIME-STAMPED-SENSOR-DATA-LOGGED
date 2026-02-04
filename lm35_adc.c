/*===============================================================
File: lm35_adc.c
Purpose: Configure ADC of the LPC21xx ARM7 MCU to read LM35 temperature
sensor using single channel (CH1) and differential channels (CH1).
Returns temperature in °C or °F without clearing UART/LCD for 1 minute
in normal operation case.
===============================================================*/

//Header with LPC21xx register definitions
#include<LPC21xx.h>

//Header file containing project-specific definitions & prototypes
#include "Mini_headers.h"

//---------------------------------------------------------
// ADC channel select masks for PINSEL1 register
// Used to configure P0.27 - P0.30 as AIN0 - AIN3 pins
//---------------------------------------------------------

u32 adcChSel[4]={AIN0_PIN_0_27,
                 AIN1_PIN_0_28,
                 AIN2_PIN_0_29,
                 AIN3_PIN_0_30
                };

//------------------------------------------------------------
// Function: Init_ADC
// Purpose : Initialize ADC for selected channel on AIN pins
// Argument: chNo - ADC channel number (0–3)
//------------------------------------------------------------
								
void Init_ADC(u32 chNo)
{
			// Clear previous configuration of AIN pins in PINSEL1
			PINSEL1&=~(adcChSel[chNo]);
			
			// Configure required AIN pin for ADC peripheral function
			PINSEL1|=adcChSel[chNo];
			
			// Clear ADC Control Register (ADCR) before configuration
			ADCR = 0;

			// Select ADC channel
			ADCR |= (1 << chNo);
	
			// Configure ADC power enable & clock divider
			ADCR|=(1<<PDN_BIT)|(CLKDIV<<CLKDIV_BITS);
}

//------------------------------------------------------------
// Function: Read_ADC
// Purpose : Start ADC conversion, wait for completion & read digital value
// Arguments: 
// chNo     - ADC channel number
// eAR      - Pointer to store converted analog result (voltage)
// adcDVal  - Pointer to store 10-bit ADC digital value
//------------------------------------------------------------

void Read_ADC(u32 chNo,f32 *eAR,u32 *adcDVal)
{
			// Clear previous channel selection bits
			ADCR&=0xFFFFFF00;
	
			// Start ADC conversion on selected channel
			ADCR|=((1<<ADC_CONV_START_BIT)|(1<<chNo));
	
			// Wait small time for ADC sampling
			delay_us(3);
	
			// Wait until conversion is completed (DONE bit becomes 1)
			while(((ADDR>>DONE_BIT)&1)==0);
	
			// Stop ADC conversion
			ADCR&=~(1<<ADC_CONV_START_BIT);
	
			// Read 10-bit digital data from ADC result register
			*adcDVal=((ADDR>>DIGITAL_DATA_BITS)&1023);
			
			// Convert digital value into equivalent analog voltage (0–3.3V)
			*eAR=*adcDVal * (3.3/1023);
}

//------------------------------------------------------------
// Function: Read_LM35_NP
// Purpose : Read LM35 temperature using differential ADC channels (CH1)
// Argument: tType - 'C' for Celsius, 'F' for Fahrenheit
// Return  : Temperature in selected format
//------------------------------------------------------------

f32 Read_LM35_NP(u8 tType)
{
			u32 adcDVal1; 
			f32 eAR1,tDeg;
			
			// Read ADC values from channel  1
			Read_ADC(CH1,&eAR1,&adcDVal1);
		
			
			// Compute differential voltage & convert into temperature
			tDeg=eAR1* 100;
		
			return tDeg;
}


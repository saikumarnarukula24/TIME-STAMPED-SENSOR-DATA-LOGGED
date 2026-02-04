//adc_defines.h
#define FOSC 12000000

#define PCLK (CCLK/4)
#define ADCCLK 3000000
#define CLKDIV ((PCLK/ADCCLK)-1)

//defines for ADCR sfr
#define CLKDIV_BITS        8 //@8-15
#define PDN_BIT            21
#define ADC_CONV_START_BIT 24

//defines for ADDR sfr
#define DIGITAL_DATA_BITS 6//@6-15
#define DONE_BIT          31

#define AIN0_PIN_0_27 0x00400000
#define AIN1_PIN_0_28 0x01000000
#define AIN2_PIN_0_29 0x04000000
#define AIN3_PIN_0_30 0x10000000

#define CH0 0
#define CH1 1
#define CH2 2
#define CH3 3
//add mores defines as & when required

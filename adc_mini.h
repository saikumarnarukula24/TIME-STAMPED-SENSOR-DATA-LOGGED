//adc.h
#include "types.h"
void Init_ADC(u32 chNo);
void Read_ADC(u32 chNo,f32 *eAR,u32 *adcDVal);

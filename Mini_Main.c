/*===============================================================
File    : main.c
Purpose :
   - Entry point of the application
   - Initializes all hardware peripherals
   - Transfers control to main system logic
===============================================================*/

#include<LPC21xx.h>
#include "Mini_headers.h"

/*---------------------------------------------------------------
 Function : main
 Purpose  :
   - Initialize UART for serial communication
   - Initialize RTC for date and time keeping
   - Initialize ADC for LM35 temperature sensing
   - Initialize LCD for user display
   - Initialize keypad for user input
   - Call System_Init() to start main application loop
----------------------------------------------------------------*/
int  main()
{
	
    InitUART();     // Initialize UART for debugging & logging
    RTC_Init();     // Initialize Real Time Clock module
    Init_ADC(CH1);  // Initialize ADC Channel 1 for LM35 sensor
    InitLCD();      // Initialize LCD display
    KeyPdInit();    // Initialize keypad interface
    System_Init();  // Start main system operation (never returns)

}


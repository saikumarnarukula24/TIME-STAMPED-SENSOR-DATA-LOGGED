/*------------------------------------------------------------
File: lcd.c
Purpose: Initialize 16x2 LCD interfacing on LPC21xx ARM7 MCU (LPC2129)
using 8-bit data mode. 
LCD pin mapping:
- Data lines  : P0.2 – P0.9
- RS          : P0.10
- RW          : P0.11
- EN          : P0.12
Provides functions for LCD command, character, integer, and string display.
------------------------------------------------------------*/

// Header with LPC21xx register definitions
#include<LPC21xx.h>

// Header containing project-specific macros, typedefs & prototypes
#include "Mini_headers.h"

//------------------------------------------------------------
// Function: InitLCD
// Purpose : Configure GPIO pins for LCD and initialize in 8-bit mode
//------------------------------------------------------------

void InitLCD(void)
{
			// Configure P0.2–P0.12 as output pins for LCD functionality
			IODIR0 |= ((LCD_DAT << 2) | (1 << RS) | (1 << RW) | (1 << EN));
			
			// Initial power-on delay for LCD reset
			delay_ms(20);
	
			// Send 0x30 three times for 8-bit mode wake-up sequence
			CmdLCD(0x30);
			delay_ms(10);
			CmdLCD(0x30);
			delay_ms(1);
			CmdLCD(0x30);
			delay_ms(1);

			// Function set: 8-bit mode, 2 lines, 5x7 font
			CmdLCD(0x38);
	
			// Move cursor or display shift command
			CmdLCD(0x10);
			
			// Clear LCD screen
			CmdLCD(0x01);
			delay_ms(5);
			
			// Entry mode: Auto-increment cursor
			CmdLCD(0x06);
			
			// Display ON, cursor OFF
			CmdLCD(0x0C);
}

//------------------------------------------------------------
// Function: CmdLCD
// Purpose : Send command byte to LCD (RS = 0)
// Argument: cmd ? 8-bit command
//------------------------------------------------------------

void CmdLCD(u8 cmd)
{
	// Clear RS pin (P0.10 ? Command mode)
	IOCLR0 = 1 << RS;  // RS = P0.10
	DispLCD(cmd);
}

//------------------------------------------------------------
// Function: CharLCD
// Purpose : Send data byte to LCD (RS = 1)
// Argument: dat ? 8-bit character data
//------------------------------------------------------------

void CharLCD(u8 dat)
{
	// Set RS pin (P0.10 ? Data mode)
	IOSET0 = 1 << RS;  // RS = P0.10
	DispLCD(dat);
}

//------------------------------------------------------------
// Function: DispLCD
// Purpose : Common display routine to write 8-bit value to LCD
// Pins    : RW = 0, EN pulse generated
// Argument: val ? 8-bit value (command or data)
//------------------------------------------------------------

void DispLCD(u8 val)
{
	 // Clear RW pin (P0.11 ? Write mode)
	IOCLR0 = 1 << RW;  // RW = P0.11
	
	// Write byte to data pins using WRITEBYTE macro (kept as-is)
	WRITEBYTE(IOPIN0, 2, val); 

	// Generate enable pulse (P0.12)
	IOSET0 = 1 << EN; // EN = P0.12
	delay_ms(2);
	IOCLR0 = 1 << EN;
	delay_ms(5);
}

//------------------------------------------------------------
// Function: StrLCD
// Purpose : Display null-terminated string on LCD
// Argument: ptr ? Pointer to input string buffer
//------------------------------------------------------------

void StrLCD(u8 *ptr)
{
	while(*ptr)
		CharLCD(*ptr++);
}

//------------------------------------------------------------
// Function: IntLCD
// Purpose : Convert and display signed 32-bit integer on LCD
// Method  : Manual integer-to-ASCII conversion (itoa logic)
// Argument: num ? 32-bit signed integer
//------------------------------------------------------------

void IntLCD(s32 num)
{
	u8 a[10];
	s8 i=0;
	//itoa
	if(num==0)
		CharLCD('0');
	else
	{
		if(num<0)
		{
			num=-num;
			CharLCD('-');
		}
		while(num>0)
		{
			a[i++]=num%10+48;
			num=num/10;
		}
		for(--i;i>=0;i--)
			CharLCD(a[i]);
	}
}

//------------------------------------------------------------
// Function: DisplayTemp
// Purpose : Display 2-digit temperature value on LCD at position 0x8C
// Argument: temp ? Unsigned temperature integer
//------------------------------------------------------------

void DisplayTemp(u32 temp)
{
		// Set DDRAM address to 0x8C (2nd line temperature position)
		CmdLCD(0x8c);
	
		 // Print tens and ones digit
		CharLCD((temp/10)+48);
		CharLCD((temp%10)+48);
	
}



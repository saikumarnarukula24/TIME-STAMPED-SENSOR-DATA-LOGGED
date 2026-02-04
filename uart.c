/*------------------------------------------------------------
File: uart.c
Purpose: Initialize UART0 of the LPC21xx ARM7 MCU (LPC2129)
on pins P0.0 (TXD0) and P0.1 (RXD0). 
Provides UART transmit and receive functions for:
- Single character
- 32-bit unsigned integer
- 32-bit floating point (±ve supported)
- String transmit
------------------------------------------------------------*/

// Header with LPC21xx register definitions
#include<LPC21xx.h>

// Header containing project-specific macros, typedefs and prototypes
#include "Mini_headers.h"

//------------------------------------------------------------
// Function: InitUART
// Purpose : Configure UART0 peripheral function & baud settings
// Pins    : P0.0 = TXD0, P0.1 = RXD0 (FUNC1)
// Baud    : Configured using U0DLL and U0DLM with 8-bit data mode
//------------------------------------------------------------

void InitUART(void)
{
			// Clear P0.0 and P0.1 function select bits
			PINSEL0 &= ~0x0000000F;   
	
			// Configure P0.0 as TXD0 and P0.1 as RXD0
			PINSEL0 |=  0x00000005;   

			// Configure UART0: 8-bit word length, 1 stop bit, no parity
			U0LCR=0x03;
	
			// Enable DLAB (Divisor Latch Access Bit) for baud configuration
			U0LCR|=1<<7;
	
			// Set baud rate divisor values
			U0DLL=97;
			U0DLM=0;
	
			// Disable DLAB after configuration
			U0LCR&=~(1<<7);
}

//------------------------------------------------------------
// Function: UARTRxChar
// Purpose : Receive a single character from UART0
// Logic   : Wait until RDR (Receiver Data Ready) bit is set
// Return  : 8-bit received character from U0RBR register
//------------------------------------------------------------

s8 UARTRxChar(void)
{
		while(!READBIT(U0LSR,0));
		return (U0RBR);

}

//------------------------------------------------------------
// Function: UARTTxChar
// Purpose : Transmit a single character via UART0
// Logic   : Load U0THR and wait until THRE (Transmit Holding
//           Register Empty) bit confirms transmission
//------------------------------------------------------------

void UARTTxChar(s8 ch)
{
	U0THR=ch;
	while(!READBIT(U0LSR,6));
}

//------------------------------------------------------------
// Function: UARTTxStr
// Purpose : Transmit a null-terminated string via UART0
// Method  : Iterative character transmit using UARTTxChar()
// Argument: ptr ? Pointer to input string buffer
//------------------------------------------------------------

void UARTTxStr(s8 *ptr)
{
	while(*ptr)
		UARTTxChar(*ptr++);
}

//------------------------------------------------------------
// Function: UARTTxU32
// Purpose : Convert and transmit a 32-bit unsigned integer
// Method  : Manual integer-to-ASCII conversion (itoa logic)
// Argument: num ? 32-bit unsigned integer
//------------------------------------------------------------

void UARTTxU32(u32 num)
{	
	u8 a[10];
	s8 i=0;
	//itoa
	if(num==0)
		UARTTxChar('0');
	else
	{
		while(num>0)
		{
			a[i++]=num%10+48;
			num=num/10;
		}
		for(--i;i>=0;i--)
			UARTTxChar(a[i]);
	}	
}

//------------------------------------------------------------
// Function: UARTTxF32
// Purpose : Convert and transmit a 32-bit floating point number
// Logic   : Supports negative values, prints 6 decimal places
// Method  : Integer part via UARTTxU32(), fractional part by
//           repeated extraction and ASCII conversion
// Argument: fnum ? 32-bit floating point number
//------------------------------------------------------------

void UARTTxF32(f32 fnum)
{
	u32 num,i;
	if(fnum<0)
	{
		UARTTxChar('-');
		fnum=-fnum;
	}
	num=fnum;
	UARTTxU32(num);
	UARTTxChar('.');
	for(i=0;i<6;i++)
	{
		fnum=(fnum-num)*10;
		num=fnum;
		UARTTxChar(num+48);
	}
}




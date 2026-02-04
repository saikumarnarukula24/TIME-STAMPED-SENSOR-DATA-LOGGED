/*===============================================================
File: Keypad.c
Purpose: Configure 4x4 keypad interfacing on LPC21xx ARM7 MCU
using GPIO port P1.16–P1.19 (Rows as output) and P1.20–P1.23
(Columns as input). Provides key detection and key value read
functions with debounce handling.
===============================================================*/

// Header with LPC21xx register definitions
#include<LPC21xx.h>

// Header file containing project-specific pin macros & prototypes
#include "Mini_headers.h"

//---------------------------------------------------------
// Function: KeyPdInit
// Purpose : Initialize GPIO pins for keypad rows as output
//           and clear row pins to logic LOW (0)
//---------------------------------------------------------

void KeyPdInit(void)
{
		// Configure P1.24–P1.27 as output pins for keypad ROW control
		IODIR1|=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));
	
	  // Clear ROW pins to 0 (initialize all rows LOW)
		IOCLR1=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));
}

//---------------------------------------------------------
// Keypad lookup table for 4x4 matrix
// Stores mapped key values for each row/column intersection
//---------------------------------------------------------

u8 KEY[][4]={1,2,3,4,
             5,6,7,8,
             9,0,11,12,
             13,14,15,16};

//------------------------------------------------------------
// Function: ColStat
// Purpose : Detect if any key is pressed by reading column pins
// Pins    : Columns are mapped on P1.20–P1.23
// Logic   : If all columns read as HIGH (0x0F), no key pressed
// Return  : 1 ? Key is pressed, 0 ? No key pressed
//------------------------------------------------------------
						 
u8 ColStat(void)
{
    return ((((IOPIN1 >> 20) & 0x0F) != 0x0F) ? 1 : 0);
}

//------------------------------------------------------------
// Function: KeyVal
// Purpose : Identify pressed key position (Row, Column) and return
//           corresponding value from lookup table
// Method  : Sequential row scanning and column status check
// Return  : Keypad decoded value (0–16)
//------------------------------------------------------------

u8 KeyVal(void)
{
		char row_val=0,col_val=0;
	
		// Scan ROW0
		IOCLR1=(1<<R0);
		IOSET1=((1<<R1)|(1<<R2)|(1<<R3));
		if((((IOPIN1>>20)&0x0f)!=0x0f))
		{
			row_val=0;
			goto colcheck;
		}
		
		// Scan ROW1
		IOCLR1=(1<<R1);
		IOSET1=((1<<R0)|(1<<R2)|(1<<R3));
		if((((IOPIN1>>20)&0x0f)!=0x0f))
		{
			row_val=1;
			goto colcheck;
		}
		
		// Scan ROW2
		IOCLR1=(1<<R2);
		IOSET1=((1<<R0)|(1<<R1)|(1<<R3));
		if((((IOPIN1>>20)&0x0f)!=0x0f))
		{
			row_val=2;
			goto colcheck;
		}
		
		// Scan ROW3
		IOCLR1=(1<<R3);
		IOSET1=((1<<R0)|(1<<R1)|(1<<R2));
		if((((IOPIN1>>20)&0x0f)!=0x0f))
			row_val=3;
		
		colcheck:
		
		// Detect which column is LOW (pressed)
		if(((IOPIN1>>C0)&1)==0)
			col_val=0;
		else if(((IOPIN1>>C1)&1)==0)
			col_val=1;
		else if(((IOPIN1>>C2)&1)==0)
			col_val=2;
		else
			col_val=3;

		// Clear all rows after scanning
		IOCLR1=((1<<R0)|(1<<R1)|(1<<R2)|(1<<R3));

		  // Return key from lookup table
		return(KEY[row_val][col_val]);
}

//------------------------------------------------------------
// Function: GetKeyPress
// Purpose : Wait for key press, read key value, and wait for release
//           Implements debounce delay before press and after release
// Return  : Decoded key value (integer)
//------------------------------------------------------------

int GetKeyPress(void)
{
    int key;

    // Wait until any key is pressed
    while(!ColStat());  

		// Debounce delay to avoid false triggering
    delay_ms(20);       

    // Read key value from scanned row/column
    key = KeyVal();

    // Wait until key is released
    while(ColStat()); 

		// Debounce delay after release
    delay_ms(20);           

    return key;
}



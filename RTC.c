/*===============================================================
File: RTC.c
Purpose: Initialize RTC of the LPC21xx ARM7 MCU (LPC2129),
configure prescalers, and provide time/date/day read & write
functions. Displays:
- Time in HH:MM:SS format
- Date in DD/MM/YYYY format
- Day of the week (SUN–SAT)
on a 16x2 LCD using existing LCD driver functions.
===============================================================*/

// Header with LPC21xx register definitions
#include<LPC21xx.h>

// Header containing project-specific macros, typedefs & prototypes
#include "Mini_headers.h"

//---------------------------------------------------------
// Array storing 3-character names for days of the week
//---------------------------------------------------------
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};


//------------------------------------------------------------
// Function: RTC_Init
// Purpose : Reset and enable RTC hardware, set clock prescalers
//------------------------------------------------------------
void RTC_Init(void) 
{
  // Reset RTC (disable + reset counter)
	CCR = RTC_RESET;
  
	#ifdef _LPC2148
		// Enable the RTC & select the clock source
		CCR = RTC_ENABLE | RTC_CLKSRC;  
	#else
	
  // Configure prescaler values for 1Hz RTC tick
	PREINT = PREINT_VAL;
	PREFRAC= PREFRAC_VAL;
  
  // Enable RTC counter
	CCR = RTC_ENABLE;  
	
	#endif
}

//------------------------------------------------------------
// Function: U32LCD
// Purpose : Display 4-digit unsigned integer on LCD (0000–9999)
// Argument: num ? 32-bit unsigned integer
//------------------------------------------------------------

void U32LCD(u32 num)
{
    CharLCD((num/1000)%10+48);  // thousands
    CharLCD((num/100)%10+48);  // hundreds
    CharLCD((num/10)%10+48);  // tens
    CharLCD((num%10)+48); // ones
}

//------------------------------------------------------------
// Function: GetRTCTimeInfo
// Purpose : Read current time from RTC registers
// Arguments: hour, minute, second (pointers to store values)
//------------------------------------------------------------

void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
	*hour = HOUR;
	*minute = MIN;
	*second = SEC;
}

//------------------------------------------------------------
// Function: SetRTCTimeInfo
// Purpose : Write new time into RTC registers
// Arguments: hour (0–23), minute (0–59), second (0–59)
//------------------------------------------------------------

void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
	HOUR = hour;
	MIN = minute;
	SEC = second;
}

//------------------------------------------------------------
// Function: DisplayRTCTime
// Purpose : Display time on LCD in HH:MM:SS format
// Arguments: hour (0–23), minute (0–59), second (0–59)
//------------------------------------------------------------

void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
		// Set cursor to 1st line start
		CmdLCD(0x80);
	
		// Print hour
		CharLCD((hour/10)+48);
		CharLCD((hour%10)+48);
	
		// Separator
		CharLCD(':');
	
		// Print minute
		CharLCD((minute/10)+48);
		CharLCD((minute%10)+48);
	
		// Separator
		CharLCD(':');
	
		// Print second
		CharLCD((second/10)+48);
		CharLCD((second%10)+48);
}

//------------------------------------------------------------
// Function: GetRTCDateInfo
// Purpose : Read current date from RTC registers
// Arguments: date, month, year (pointers to store values)
//------------------------------------------------------------

void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
	*date = DOM;
	*month = MONTH;
	*year = YEAR;
}


//------------------------------------------------------------
// Function: SetRTCDateInfo
// Purpose : Write new date into RTC registers
// Arguments: date (1–31), month (1–12), year (4-digit)
//------------------------------------------------------------

void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
	DOM = date;
	MONTH = month;
	YEAR = year;	
}

//------------------------------------------------------------
// Function: DisplayRTCDate
// Purpose : Display date on LCD in DD/MM/YYYY format
// Arguments: date (1–31), month (1–12), year (4-digit)
//------------------------------------------------------------

void DisplayRTCDate(u32 date, u32 month, u32 year)
{
		// Set cursor to 2nd line start
		CmdLCD(0xC0);
	
		 // Print date
		CharLCD((date/10)+48);
		CharLCD((date%10)+48);
	
		 // Separator
		CharLCD('/');
	
		// Print month
		CharLCD((month/10)+48);
		CharLCD((month%10)+48);
		
		 // Separator
		CharLCD('/');
	
		// Print year
		U32LCD(year);
}


//------------------------------------------------------------
// Function: GetRTCDay
// Purpose : Read current day of week from RTC register
// Argument: day (pointer to store 0=SUN … 6=SAT)
//------------------------------------------------------------

void GetRTCDay(s32 *day)
{
	*day = DOW; 
}

//------------------------------------------------------------
// Function: SetRTCDay
// Purpose : Write new day of week into RTC register
// Argument: day (0=SUN … 6=SAT)
//------------------------------------------------------------
void SetRTCDay(u32 day)
{
	DOW = day;
}

//------------------------------------------------------------
// Function: DisplayRTCDay
// Purpose : Display day of week string on LCD
// Argument: dow (0=SUN … 6=SAT)
//------------------------------------------------------------

void DisplayRTCDay(u32 dow)
{
	// Set cursor near end of 2nd line (day position)
	CmdLCD(0xcc);
	
	// Print 3-char day string from lookup table
	StrLCD((u8*)week[dow]);  
}




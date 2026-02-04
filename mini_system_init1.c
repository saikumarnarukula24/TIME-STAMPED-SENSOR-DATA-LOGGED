/*===============================================================
File    : mini_system_init1.c
Purpose : Real-time Temperature Monitoring System using LPC21xx
          - RTC date & time display
          - LM35 temperature sensing
          - UART logging with timestamp
          - Over-temperature alert with LED
          - Editable RTC & temperature setpoint via keypad
===============================================================*/
#include<LPC21xx.h>
#include "Mini_headers.h"


/*--------------------------------------------------------------------
Global Variables
--------------------------------------------------------------------*/
s32 hour,min,sec,date,month,year,day;
extern u8 KEY[4][4];// Keypad matrix
extern char week[][4];// Weekday strings (SUN, MON, ...)



/*--------------------------------------------------------------------
Function: System_Init
Purpose : 
    - Initialize RTC with default values
    - Read & display temperature continuously
    - Log data through UART
    - Handle keypad based edit modes
--------------------------------------------------------------------*/
void System_Init(void)
{
		float currentTemp;// Stores current temperature
		int setpoint=46;// Default temperature value
		int cnt=0;//To prevent repeated UART prints
		int	printonce=0;
		s32 prevs=-1;
		s32 prevs1=-1;

	
		
/*------------------------------------------------------------
      Initialize RTC with default date and time
 ------------------------------------------------------------*/
		SetRTCTimeInfo(12,14,00);// HH:MM:SS format
		SetRTCDateInfo(30,10,2025);// DD/MM/YYYY format
		SetRTCDay(4);

/*------------------------------------------------------------
      Startup message on UART
 ------------------------------------------------------------*/
	if(printonce==0)
	{
	  UARTTxStr("LM35 TEST: \n\r");
		UARTTxStr("Temperature : ");
		UARTTxF32(Read_LM35_NP('C'));
		UARTTxStr("\xF8"); 
					UARTTxStr("C\n\r");
	}
	
/*------------------------------------------------------------
      Main Super Loop
 ------------------------------------------------------------*/
		while(1)
		{
					int pres=0,prev=1;
					
/*--------------------------------------------------------
          Read RTC values
 --------------------------------------------------------*/
					INPUT:	
					GetRTCTimeInfo(&hour,&min,&sec);
					GetRTCDateInfo(&date,&month,&year);
					GetRTCDay(&day);
					
/*--------------------------------------------------------
          Display RTC on LCD
 --------------------------------------------------------*/
					DisplayRTCTime(hour,min,sec);
					DisplayRTCDate(date,month,year);
					DisplayRTCDay(day);
					
/*--------------------------------------------------------
          Read and display temperature
 --------------------------------------------------------*/
					currentTemp = Read_LM35_NP('C'); 
					
					DisplayTemp((u32)currentTemp);
					CharLCD(0xDF);// Degree symbol
					CharLCD('C');	
					
/*--------------------------------------------------------
          Temperature Normal Condition
 --------------------------------------------------------*/
					if((currentTemp < setpoint)&&(cnt==0 || sec==0)) 
					{ 
						cnt++;
						IODIR1|=(1<<LED);// Configure the LED pin as an output pin
						IOCLR1 = 1<<LED;// Clearing the LED pin
						
				
					if(sec != prevs1)
					{
						prevs1=sec;
					UARTTxStr("Temp: "); 
					UARTTxF32(currentTemp); 
					UARTTxStr("\xF8"); 
					UARTTxStr("C @ ");
					 /* Print date */
					UARTTxStr(" ");
					if(hour<10) 
						UARTTxChar('0'); 
					UARTTxU32(hour); 
					UARTTxChar(':');
					if(min<10)  
						UARTTxChar('0'); 
					UARTTxU32(min);  
					UARTTxChar(':');
					if(sec<10)  
						UARTTxChar('0'); 
					UARTTxU32(sec);
					UARTTxStr(" ");
					
					if(date < 10) 
						UARTTxStr("0"); 
					UARTTxU32(date); 
					UARTTxStr("/"); 
					if(month < 10) 
						UARTTxStr("0"); 
					UARTTxU32(month); 
					UARTTxStr("/");
					UARTTxU32(year); 
					UARTTxStr("\n\r");
					}
					
				}
/*--------------------------------------------------------
          Periodic Logging (Every Minute)
 --------------------------------------------------------*/
					

/*--------------------------------------------------------
          Over-Temperature Condition
 --------------------------------------------------------*/					
					else if(currentTemp > setpoint) 
					{ 
						cnt=0;// Reset normal print flag
					if(sec != prevs)
					{
						prevs=sec;
					
							UARTTxStr("[ALERT!] "); 
					UARTTxStr("Temp: "); 
					UARTTxF32(currentTemp); 
					UARTTxStr("\xF8"); 
					UARTTxStr("C @ ");

					UARTTxStr(" ");
					if(hour<10) 
						UARTTxChar('0'); 
					UARTTxU32(hour); 
					UARTTxChar(':');
					if(min<10)  
						UARTTxChar('0'); 
					UARTTxU32(min);  
					UARTTxChar(':');
					if(sec<10)  
						UARTTxChar('0'); 
					UARTTxU32(sec);
					UARTTxStr(" ");
					
					if(date < 10) 
						UARTTxStr("0"); 
					UARTTxU32(date); 
					UARTTxStr("/"); 
					if(month < 10) 
						UARTTxStr("0"); 
					UARTTxU32(month); 
					UARTTxStr("/");
					UARTTxU32(year); 
					UARTTxStr(" - OVER TEMP");
					UARTTxStr("\n\r");					
					
						IODIR1|=(1<<LED);
						IOCLR1 = 1<<LED;
						IOSET1 = 1<<LED; // LED ON at P0.13 
	
					}
				}
/*--------------------------------------------------------
          Switch Handling for Edit Mode
 --------------------------------------------------------*/
					PINSEL2 &= ~(1<<SW);
					IODIR1 &= ~(1<<SW);
			
					pres = (((IOPIN1 >> SW)&1)==0);
/*--------------------------------------------------------
          Enter Edit Mode on Switch Press
 --------------------------------------------------------*/					
					if(pres && prev)
					{
						delay_ms(50);// Debounce delay
						if(pres && prev)
						{
							
							IN1:
							CmdLCD(0x01);//clear lcd
							CmdLCD(0x80);
							StrLCD("1.EDIT RTC INFO");
							CmdLCD(0xC0);
							StrLCD("2.E.SET");
							CmdLCD(0xCA);
							StrLCD("3.EXT");
							
							UARTTxStr("***EDIT MODE ACTIVATED***\n\r");
						
/*------------------------------------------------
                  Edit Menu Loop
 ------------------------------------------------*/							
							while(1)
							{
								int opt,field,press=0;
								//wait for sw press
								delay_ms(10);//for avoiding keybouncing time.
								
								opt = GetKeyPress();

/*=============================================================
                  RTC Date & Time Editing Mode
===============================================================
 Option 1 : RTC Date & Time Editing Mode
 Purpose :
   - Allows user to modify RTC parameters using keypad
			 15 -> Increment
       10 -> Decrement
       13 -> Exit / Save
   - Editable fields:
       • Hour
       • Minute
       • Second
       • Date
       • Month
       • Year
       • Day of week
   - Updated values are saved into RTC registers
   - LCD provides live feedback during editing
===============================================================*/

								if(opt ==1)
								{
									IN:
									CmdLCD(0x01);//Clear LCD
									CmdLCD(0x80);
									StrLCD("1.H 2.MI 3.S 4.D");
									CmdLCD(0xC0);
									StrLCD("5.M 6.Y 7.DY 8.E");

									
									if(press == 0)
									{
										UARTTxStr("***** Time Editing Mode Activated *****\n\r");
										press++;
									}
									
									field =GetKeyPress();
									if(field == 15 || field == 14 || field == 13 || field == 12 || field ==11 || field ==10 || field ==0 )
										goto IN;// Ignore invalid selection
									
									
/*---------------------------------------------------------------
Field 1 : Hour Editing
Range : 0 – 23
----------------------------------------------------------------*/
									if(field == 1)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET HOUR:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase hour
												{
														hour++;
														if(hour==24)
														{
															hour=0;
														}
												}
												else if (Key == 16) // Decrease hour
												{
																hour--;
													if(hour < 0)
														hour=23;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCTimeInfo(hour, min, sec);
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												CmdLCD(0x8A);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												IntLCD(hour);// Show updated hour

												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
									
/*---------------------------------------------------------------
 Field 2 : Minute Editing
 Range : 0 – 59
----------------------------------------------------------------*/									
									else if(field == 2)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET MIN:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase min
												{
														min++;
														if(min==60)
														{
															min=0;
														}
												}
												else if (Key == 16) // Decrease min
												{
													min--;
													if(min < 0)
														min=59;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCTimeInfo(hour, min, sec);
														CmdLCD(0x01);//clear lcd
														goto IN;
												}

												CmdLCD(0x8A);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												IntLCD(min);// Show updated minutes

												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
									
/*---------------------------------------------------------------
 Field 3 : Second Editing
 Range : 0 – 59
----------------------------------------------------------------*/
									else if(field == 3)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET SEC:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase sec
												{
														sec++;
														if(sec==60)
														{
															sec=0;
														}
												}
												else if (Key == 16) // Decrease sec
												{
													sec--;
													if(sec < 0)
														sec=59;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCTimeInfo(hour, min, sec);
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												CmdLCD(0x8A);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												IntLCD(sec);// Show updated seconds

												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
									
/*---------------------------------------------------------------
 Field 4 : Date Editing
 Includes month & leap-year validation
----------------------------------------------------------------*/
									else if(field == 4)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET DATE:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase date
												{
													date++;
												}
												else if (Key == 16) // Decrease date
												{
													date--;
												}
												else if (Key  == 13) // Exit edit mode
												{
														// ----- VALIDATION CHECK -----
														int valid = 1;
														int maxDate;

														if(month == 2)
														{
																if(year % 4 == 0)
																		maxDate = 29;
																else
																		maxDate = 28;
														}
														else if(month == 4 || month == 6 || month == 9 || month == 11)
																maxDate = 30;
														else
																maxDate = 31;

														if(date < 1 || date > maxDate)
																valid = 0;

														// ----- INVALID WARNING -----
														if(!valid)
														{
																CmdLCD(0x01);//Clear LCD
																CmdLCD(0x80);
																StrLCD("? INVALID DATE!");
																CmdLCD(0xC0);
																StrLCD("Not Saved");
																UARTTxStr("[WARN] Invalid Date Entered, Not Saved!\n\r");
																delay_ms(1200);
																CmdLCD(0x01);//Clear LCD
																goto IN; // return without saving
														}

														// ----- SAVE ONLY IF VALID -----
														SetRTCDateInfo(date, month, year);
														UARTTxStr("[OK] Date Updated Successfully!\n\r");
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												// ----- CYCLIC LIMITS DURING EDIT -----
												if(month == 2)
												{
														if(year % 4 == 0)
														{
																if(date > 29) date = 1;
																if(date < 1)  date = 29;
														}
														else
														{
																if(date > 28) date = 1;
																if(date < 1)  date = 28;
														}
												}
												else if(month == 4 || month == 6 || month == 9 || month == 11)
												{
														if(date > 30) date = 1;
														if(date < 1)  date = 30;
												}
												else
												{
														if(date > 31) date = 1;
														if(date < 1)  date = 31;
												}

												// ----- DISPLAY UPDATED DATE -----
												CmdLCD(0x8A);
												StrLCD("  ");
												CmdLCD(0x8A);
												IntLCD(date);

												delay_ms(200);
										}
									}
/*---------------------------------------------------------------
 Field 5 : Month Editing
 Range : 1 – 12
----------------------------------------------------------------*/									
									else if(field == 5)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET MON:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase month
												{
														month++;
														if(month==13)
														{
															month=1;
														}
												}
												else if (Key == 16) // Decrease month
												{
													month--;
													if(month < 0)
														month=12;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCDateInfo(date,month,year);
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												CmdLCD(0x8A);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												IntLCD(month);// Show updated minutes

												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
/*---------------------------------------------------------------
 Field 6 : Year Editing
----------------------------------------------------------------*/									
									else if(field == 6)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET YEAR:");
										
										
										while (1)
										{
											
											int Key=KeyVal();
												if (Key == 15)      // Increase year
												{
														year++;
												}
												else if (Key == 16) // Decrease year
												{
													year--;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCDateInfo(date,month,year);
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												CmdLCD(0x8A);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												IntLCD(year);;// Show updated minutes
												
												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
									
/*---------------------------------------------------------------
 Field 7 : Day of Week Editing
 Range : 0 – 6
----------------------------------------------------------------*/									
									else if(field == 7)
									{
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET DAY:");
										
										
										while (1)
										{
											int Key=KeyVal();
												if (Key == 15)      // Increase day
												{
														day++;
														if(day==7)
														{
															day=0;
														}
												}
												else if (Key == 16) // Decrease day
												{
													day--;
													if(day < 0)
														day=6;
												}
												else if (Key  == 13) // Exit edit mode
												{
														SetRTCDay(day);
														CmdLCD(0x01);//Clear LCD
														goto IN;
												}

												CmdLCD(0x88);       // 1st line
												StrLCD("  ");       // small space
												CmdLCD(0x8A);  
												StrLCD((u8*)week[day]);// Show updated minutes

												delay_ms(200);     // small delay to avoid fast repeat
										}	
									}
									
/*---------------------------------------------------------------
 Field 8 : Exit Time Editing Mode
----------------------------------------------------------------*/
									else if(field == 8)
									{
										CmdLCD(0x01);//Clear LCD
									  goto IN1;
									}
								}	
									
/*=============================================================
            Temperature Setpoint Editing Mode
===============================================================

 Option 2 : Temperature Setpoint Editing Mode
 Purpose :
   - Allows user to modify temperature threshold value
   - Setpoint is used for:
       • Over-temperature detection
       • LED alert control
       • UART alert messages
   - Keypad controls:
       • Increment / Decrement setpoint
       • Exit to main menu
   - LCD displays updated setpoint in real-time
===============================================================*/
									else if (opt == 2)  // Enter Edit Mode
									{
										int press1=0;
										
										CmdLCD(0x01);       // Clear LCD
										CmdLCD(0x80);       // 1st line start
										StrLCD("SET TEMP:");
										CmdLCD(0xC0);       // 2nd line
										CharLCD(' ');       // small space
										DisplayTemp(setpoint); // Show updated setpoint
										CharLCD(0xDF);     // ° symbol
										CharLCD('C');
										
									if(press1 == 0)
									{
										UARTTxStr("*** Set Point Editing Mode Activated ***\n\r");
										press1++;
									}

										while (1)
										{
												int Key = GetKeyPress();
												if (Key == 15)      // Increase setpoint
												{
														setpoint++;
												}
												else if (Key == 16) // Decrease setpoint
												{
														if(setpoint > 0)
																setpoint--;
												}
												else if (Key == 13) // Exit edit mode
												{
														CmdLCD(0x01);
														goto IN1;
												}

												CmdLCD(0xC0);       // 2nd line
												CharLCD(' ');       // small space
												DisplayTemp(setpoint); // Show updated setpoint
												CharLCD(0xDF);     // ° symbol
												CharLCD('C');

												delay_ms(200);     // small delay to avoid fast repeat
									  }
								  }
/*=============================================================
                       Exit Edit Mode
===============================================================
 Option 3 : Exit Edit Mode
 Purpose :
   - Clears LCD display
   - Returns execution to normal monitoring mode
   - RTC display and temperature monitoring resumes
===============================================================*/
								  else if(opt == 3)
								  {
									  CmdLCD(0x01);//Clear LCD
									  goto INPUT;
								  }
							  }							
					    }
			      }
	       }
		
}

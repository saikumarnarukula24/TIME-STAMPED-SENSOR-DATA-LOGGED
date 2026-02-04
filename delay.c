/*===============================================================
 File: delay.c
 Functions: delay_us,delay_ms,delay_s.
 Purpose:
   - Provide simple blocking delay routines for LPC21xx
   - Implement microsecond, millisecond, and second delays
   - Useful for timing, debouncing, and sensor read intervals
===============================================================*/
void delay_us(unsigned int tdly)
{
	tdly*=12;
	while(tdly--);
}
void delay_ms(unsigned int tdly)
{
	tdly*=12000;
	while(tdly--);
}
void delay_s(unsigned int tdly)
{
	tdly*=12000000;
	while(tdly--);
}

#include <avr/io.h>
#include "bit.h"
#include "io.c"
#include <avr/interrupt.h>

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);

	LCD_DisplayString(1, "Congratulations!");
}
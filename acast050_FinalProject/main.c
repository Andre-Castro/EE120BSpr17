/*
 * acast050_FinalProject.c
 *
 * Created: 5/20/2017 3:17:38 PM
 * Author : Andre
 */ 

#include <avr/io.h>
#include "io.c"

const unsigned char ARR_SIZE = 5;
char* strings[ARR_SIZE] = {"Channel 1", "Channel 2", "Channel 3", "Channel 4", "Channel 5"}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

enum JS_STATES {init, up, down};
void joyStick_Tick(){
	
}

int main(void)
{
	unsigned short x = ADC;
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	ADC_init();
	unsigned short temp = x;
	unsigned short orig = x;
    while (1){
		x = ADC;
		if(x != temp || x != orig){
			temp = x;
		}
		PORTB = temp & 0xFF;
    }
}


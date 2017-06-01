/*
 * acast050_FinalProject.c
 *
 * Created: 5/20/2017 3:17:38 PM
 * Author : Andre
 */ 
#include <avr/io.h>
#include "io.c"

const unsigned char ARR_SIZE = 5;
const char *strings[5] = {"Channel 1", "Channel 2", "Channel 3", "Channel 4", "Channel 5"};


volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}


ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

enum JS_STATES {init, up, down} JS_STATE;
void joyStick_Tick(){
	static unsigned char pos = 0;
	unsigned char J = ~(PINC & 0x01);
	switch(JS_STATE){ //transitions
		case init:
			if(J == 1 && pos < ARR_SIZE){
				JS_STATE = down;
			}
			else{
				JS_STATE = init;
			}
			break;
		case up:
			break;
		case down:
			break;
		default:
			JS_STATE = init;
			break;
	}
	switch(JS_STATE){ //actions
		case init:
			break;
		case up:
			break;
		case down:
			pos++;
			LCD_DisplayString(1, strings[pos]);
			break;
		default:
			LCD_DisplayString(1, strings[0]);
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0x00; PORTC = 0xFF; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
		
	unsigned long GCD = 1;
	TimerSet(GCD);
	TimerOn();

	LCD_DisplayString(1, strings[0]);
	
	unsigned short x = ADC;

	while (1){
		joyStick_Tick();
		x = ADC;
		PORTB = x & 0xFF;
	}
	return 0;
}

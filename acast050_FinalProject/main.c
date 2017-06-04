/*
 * acast050_FinalProject.c
 *
 * Created: 5/20/2017 3:17:38 PM
 * Author : Andre
 */ 
#include <avr/io.h>
#include "io.c"
#include "bit.h"

unsigned char pulses[100][2];
const unsigned short MAX_VAL = 65000;

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

typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

task keypad;
task tasks[1];
const unsigned char taskNum = 1;

enum JS_STATES {init, up, down, maintainD, maintainU} JS_STATE;
void joyStick_Tick(){
	static unsigned char pos = 0;
	unsigned char J = ~PINA & 0x08;
	switch(JS_STATE){ //transitions
		case init:
			if(J > 0 && pos < ARR_SIZE - 1){
				JS_STATE = down;
			}
			else{
				JS_STATE = init;
			}
			break;
		case up:
			break;
		case down:
			JS_STATE = maintainD;
			break;
		case maintainD:
			if(J == 0){
				JS_STATE = init;
			}
			else{
				JS_STATE = maintainD;
			}
			break;
		case maintainU:
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
		case maintainD:
			break;
		case maintainU:
			break;
		default:
			LCD_DisplayString(1, strings[0]);
			break;
	}
}

void irLoop(){
	unsigned short highPulse = 0;
	unsigned short lowPulse = 0;
	static unsigned char currentPulse = 0;
	if(PINA & 0x01){
		while(PINA & 0x01){
			highPulse++;
			if(highPulse >= MAX_VAL){
				return currentPulse;
			}
		}
		pulses[currentPulse][0] = highPulse;
		while(!(PINA & 0x01)){
			lowPulse++;
			if(lowPulse >= MAX_VAL){
				return currentPulse;
			}
		}
		pulses[currentPulse][1] = lowPulse;
		currentPulse++;
		//LCD_DisplayString(1, pulses[currentPulse]);
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0xFF; PORTB = 0x00; //Ouptut
	DDRC = 0x0F; PORTC = 0xF0; 
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
		
	unsigned long GCD = 200;
	TimerSet(GCD);
	TimerOn();

	LCD_DisplayString(1, strings[0]);
	
	unsigned short x = ADC;
	unsigned char count2 = 0;
	unsigned char nineAndTen = 0;

	while (1){
		joyStick_Tick();
		//while(count2 < 50){
			while (!TimerFlag);
			TimerFlag = 0;
			count2 ++;

			irLoop();
			LCD_DisplayString(1, count2);

			x = ADC;
			PORTB = x & 0xFF;
			nineAndTen = (char)(x>>8);
			PORTC = SetBit(PORTC, 2, (nineAndTen & 0x01));
			PORTC = SetBit(PORTC, 3, (nineAndTen & 0x02));
		//}
		//count2 = 0;
	}
	return 0;
}

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
unsigned char currentPulse = 0;

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
task tasks[2];
const unsigned char taskNum = 2;

enum JS_STATES {JS_init, up, down, maintainD, maintainU} JS_STATE;
void joyStick_Tick(){
	static unsigned char pos = 0;
	unsigned char J = ~PINA & 0x08;
	switch(JS_STATE){ //transitions
		case JS_init:
			if(J > 0 && pos < ARR_SIZE - 1){
				JS_STATE = down;
			}
			else{
				JS_STATE = JS_init;
			}
			break;
		case up:
			break;
		case down:
			JS_STATE = maintainD;
			break;
		case maintainD:
			if(J == 0){
				JS_STATE = JS_init;
			}
			else{
				JS_STATE = maintainD;
			}
			break;
		case maintainU:
			break;
		default:
			JS_STATE = JS_init;
			break;
	}
	switch(JS_STATE){ //actions
		case JS_init:
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

enum IR_STATES{IR_init, countHigh, countLow, update};
void irLoop(unsigned short x, unsigned short y){
	unsigned short highPulse = 0;
	unsigned short lowPulse = 0;
	//static unsigned char currentPulse = 0;
	if(x != ADC){ //if a change from the orriginal ADC signal is detected
		y = ADC; //set y to the current ADC signal (hopefully still different from x)
		while(x != y){ // while that signal still differs (it is high)
			y = ADC;
			highPulse++; // count how many iterations it is high for
			if(highPulse >= MAX_VAL){
				return 0;
			}
		}
		pulses[currentPulse][0] = highPulse;
		while(x == y){ // the signal has gone back to being low (or how it was orriginally before button press)
			lowPulse++; // count how many iteratinos it is low for
			y = ADC;
			if(lowPulse >= MAX_VAL){
				return 0;
			}
			//PORTB = lowPulse & 0xFF;
		}
		pulses[currentPulse][1] = lowPulse;
		currentPulse++;
		//LCD_DisplayString(1, pulses[currentPulse]);
	}
	if(currentPulse >= 100){
		currentPulse = 0;
	}
}

void decodeTick(){
	/*if(pulses[0][0] == 0){
		return; // no signal has been sent yet
	}
	else{
		if(pulses[0][0] > 100){
			LCD_DisplayString(1, "Power");
		}
	}*/
	unsigned char i = 0;
	/*for(i = 1; i < 17; i++){
		LCD_Cursor(i);
		LCD_WriteData(pulses[i][0] + ' ');
	}
	for(i = 17; i <33; i++){
		LCD_Cursor(i);
		LCD_WriteData(pulses[i][1] + ' ');
	} */

	unsigned long averageHigh = 0;
	for(i = 0; i < currentPulse; i++){
		averageHigh += pulses[i][0];
	}
	averageHigh = averageHigh / currentPulse;
	//LCD_Cursor(1);
	//LCD_WriteData(averageHigh + ' ');

	if(averageHigh > 45 && averageHigh < 57){
		LCD_DisplayString(1, "Power!");
	}
	else if(averageHigh > 77 && averageHigh < 84){
		LCD_DisplayString(1, "Channel");
	}

	/*unsigned long averageLow = 0;
	for(i = 0; i < currentPulse; i++){
		averageLow += pulses[i][1];
	}
	averageLow = averageLow / currentPulse;
	LCD_Cursor(17);
	LCD_WriteData(averageLow + ' ');*/
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
		
	unsigned long GCD = 1;
	TimerSet(GCD);
	TimerOn();

	//LCD_DisplayString(1, strings[0]);
	
	unsigned short x = ADC;
	unsigned short y = 0;
	unsigned char count2 = 0;
	unsigned char nineAndTen = 0;
	unsigned short high = 0;

	/*unsigned char i = 0;
	tasks[i].state = IR_init;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &irLoop;
	
	i++;
	tasks[i].state = JS_init;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &joyStick_Tick;

	while(1) {
		for(i = 0; i < taskNum; i++){
			if(tasks[i].elapsedTime >= tasks[i].period){
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += GCD;
		}
	}*/


	while (1){
		//joyStick_Tick();
		decodeTick();
		while(count2 < 100){ //100 readings to fill up array
			while (!TimerFlag);
			TimerFlag = 0;
			count2 ++;

			irLoop(x, y);

			/*if(x != ADC){ //ADC has changed, signal has been deteced.
				LCD_DisplayString(1, "Change detected");
				y = ADC;
				high ++;
				while(x!=y){ // while signal is still different than the original
					high++;
					y = ADC;
				}
				LCD_DisplayString(1, "New Change Deteced");
				x = ADC;
			}
			PORTB = high & 0xFF; */

			//x = ADC;
			//PORTB = x & 0xFF;
			//nineAndTen = (char)(x>>8);
			//PORTC = SetBit(PORTC, 2, (nineAndTen & 0x01));
			//PORTC = SetBit(PORTC, 3, (nineAndTen & 0x02));
		}
		count2 = 0;
	}
	return 0;
}

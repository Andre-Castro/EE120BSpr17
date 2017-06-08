/*
 * acast050_FinalProject.c
 *
 * Created: 5/20/2017 3:17:38 PM
 * Author : Andre
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include "io.c"
#include "bit.h"

//NOKIA 5110 Configuration
#define CE			PINC5	//PINB0		// SCE chip enable, active low		->	SS
#define RST			PINC6	//PINB1		// reset pin
#define DC			PINC4	//PINB2		// Mode Select, data (high)/character(low)
#define DIN			PINC3	//PINB3		// data in							->	MOSI
#define CLK			PINC2	//PINB4		// clocking the data to the LCD		->	SCLK
#define LCD_Port	PORTC
#define LCD_DataDir	DDRC

//#define DIN PINC3		//Data IN
//#define CLK PINC2		//Clock
//�#define CE  PINC5		//Chip Enable
//#define RST PINC6		//Reset
//#define DC  PINC4		//Data/Command

//#define LCD_PORT PORTC
//#define LCD_PORT_DD DDRC

#define F_CPU 1000000UL

#define Black 1
#define White 0
#define Filled 1
#define NotFilled 0

unsigned char pulses[100][2];
const unsigned short MAX_VAL = 65000;
unsigned char currentPulse = 0;

unsigned char on = 0;
unsigned char currChannel = 0;

const unsigned char ARR_SIZE = 5;
const char *strings[5] = {"Channel 1", "Channel 2", "Channel 3", "Channel 4", "Channel 5"};

 char LCDBuffer[] = {
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,0 - 15,7
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,0 - 31,7
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,0 - 47,7
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,0 - 63,7
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,0 -	79,7
	 0x00,0x00,0x00,0x00,															 //pixel 80,0 - 83,7
	 
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,7 - 15,15
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,7 - 31,15
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,7 - 47,15
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,7 - 63,15
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,7 -	79,15
	 0x00,0x00,0x00,0x00,															 //pixel 80,7 - 83,15
	 
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,16 - 15,23
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,16 - 31,23
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,16 - 47,23
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,16 - 63,23
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,16 - 79,23
	 0x00,0x00,0x00,0x00,															 //pixel 80,16 - 83,23
	 
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,24 - 15,31
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,24 - 31,31
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,24 - 47,31
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,24 - 63,31
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,24 - 79,31
	 0x00,0x00,0x00,0x00,															 //pixel 80,24 - 83,31
	 
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,32 - 15,39
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,32 - 31,39
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,32 - 47,39
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,32 - 63,39
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,32 - 79,39
	 0x00,0x00,0x00,0x00,															 //pixel 80,32 - 83,39
	 
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 0,40 - 15,47
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 16,40 - 31,47
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 32,40 - 47,47
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 48,40 - 63,47
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //pixel 64,40 - 79,47
	 0x00,0x00,0x00,0x00,															 //pixel 80,40 - 83,47
	 
 };


 char RobotHead [] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
	 0xFC, 0xFF, 0xDF, 0xAF, 0x55, 0x80, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00,
	 0x55, 0x00, 0x55, 0x00, 0x55, 0x80, 0xD5, 0x80, 0xD5, 0xE0, 0xF5, 0x60, 0x75, 0x70, 0x75, 0x60,
	 0x75, 0xE0, 0xD5, 0x80, 0xD5, 0x00, 0x55, 0x00, 0x55, 0x00, 0x57, 0x07, 0x5E, 0x3C, 0x70, 0xF0,
	 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x03, 0x5F, 0xFF, 0xFF, 0xFF, 0xD7, 0xEE, 0xD5, 0xE8, 0xD5, 0xA0, 0xD5, 0x80,
	 0xD5, 0xC0, 0xF5, 0xE0, 0xBD, 0xF8, 0xDF, 0xEE, 0x7F, 0x3B, 0x3F, 0x0F, 0x0F, 0x06, 0x06, 0x06,
	 0x06, 0x06, 0x0C, 0x0C, 0x38, 0x38, 0xF1, 0xC1, 0xC3, 0x07, 0x07, 0x3C, 0x3D, 0xF8, 0xF5, 0xC0,
	 0x55, 0x00, 0x55, 0x00, 0x55, 0x03, 0x57, 0x1E, 0x7C, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xC7, 0xFF, 0xFD, 0x7A,
	 0xDD, 0xFE, 0xFF, 0x77, 0x7F, 0x3C, 0x7E, 0x3F, 0x3F, 0x3F, 0x7F, 0xFF, 0x80, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x8F, 0xC4, 0xE0, 0x7D, 0x3F, 0x1F, 0x00, 0x00, 0x00,
	 0x80, 0x80, 0xE1, 0xE7, 0x7F, 0x3E, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x01,
	 0x57, 0x0F, 0x5C, 0x78, 0x70, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xE0, 0xF0, 0xBC, 0xDF, 0xEF,
	 0xF7, 0xFF, 0xFF, 0xFF, 0x47, 0x03, 0x01, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xE0, 0x00, 0x01,
	 0x07, 0x07, 0x0E, 0xCC, 0xCC, 0xFC, 0x7C, 0x3C, 0x1C, 0x0E, 0x07, 0x83, 0xC1, 0xE0, 0x70, 0x30,
	 0x78, 0x1C, 0x5C, 0x0F, 0x57, 0x03, 0xD5, 0xC0, 0xD5, 0xE0, 0x75, 0x30, 0x35, 0xB8, 0xDD, 0xF8,
	 0xFD, 0xF8, 0xFD, 0xB8, 0x35, 0x30, 0xF5, 0xC0, 0xD5, 0x03, 0x57, 0x0F, 0x5F, 0xBC, 0xF0, 0xF0,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF,
	 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xF0, 0xF0, 0xE1,
	 0xF1, 0xF0, 0x78, 0xB8, 0x5E, 0x2E, 0x57, 0x81, 0xD1, 0xE8, 0xF4, 0xB8, 0xFC, 0xBE, 0x77, 0xA3,
	 0x55, 0x80, 0xD5, 0xC0, 0xF5, 0x78, 0x3D, 0xBE, 0xFF, 0xE6, 0xF3, 0xF9, 0x1D, 0x0D, 0x07, 0x07,
	 0x0F, 0xF9, 0xF9, 0xF0, 0xF0, 0xB8, 0x19, 0x1F, 0xC6, 0xCE, 0xFB, 0xFB, 0x7F, 0xE8, 0xF5, 0xFE,
	 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x3B, 0x77, 0x7F, 0x7F, 0x6F, 0x7F, 0x7F,
	 0x7F, 0x3F, 0x3F, 0x3B, 0x1D, 0x1C, 0x1D, 0x1E, 0x1F, 0x0E, 0x1F, 0x0F, 0x1F, 0x3F, 0x7D, 0xEE,
	 0xFF, 0xFE, 0xF7, 0xEA, 0x7F, 0x7F, 0x7F, 0x3F, 0x1F, 0x1E, 0x7F, 0xE3, 0xC1, 0xE0, 0xF1, 0x73,
	 0x1F, 0x1E, 0x9F, 0xFB, 0xF3, 0x60, 0x70, 0xFC, 0xDD, 0xEF, 0xD7, 0xE3, 0xD7, 0xE1, 0xD5, 0xF8,
	 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 };

 char WB [] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0xFC, 0xFC, 0xFC, 0xF0, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC, 0xFC, 0xFC, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
	 0xFC, 0xFC, 0xFC, 0x1C, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0xFC, 0xFC, 0x3C, 0x3C, 0x3C, 0x3C,
	 0x3C, 0x7C, 0xFC, 0xF8, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	 0x1F, 0xFF, 0xFF, 0xFE, 0xE0, 0x00, 0xC0, 0xFE, 0xFF, 0x7F, 0x07, 0x07, 0x7F, 0xFF, 0xFE, 0xE0,
	 0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0x1F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	 0x78, 0x78, 0x78, 0x78, 0x78, 0x7C, 0xFF, 0xFF, 0xFF, 0xE3, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3F, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00,
	 0x00, 0x0F, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF8, 0xFF, 0xFF, 0x7F, 0x1F,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 };



 void SetPixel (unsigned char xp, unsigned char yp, char bw);				//draw a single pixel specify x,y location and Black/White 
 void LCDInitialize(void);													//set up the lcd
 void SendCommand_LCD (unsigned int CommandOut);							//sends a command to the lcd for control
 void LCD_clear(void);														//clear the lcd, also fills the LCDBuffer with 0's
 void LCD_gotoXY (unsigned char x, unsigned char y);						//used for characters (6 banks of y positions)
 void SendData_LCD (unsigned int DataOut);									//sends data to the lcd for displaying
 void LCD_Update (void);														//transfer LCDBuffer to the lcd
 void ReplaceBitmap (char ReplacementBitmap[]);								//fills the LCDBuffer with the bytes of a ReplacementBitmap
 void LCD_5110_init(void);
 void DrawRectangle (int x1, int y1, int x2, int y2,char bw, char fill);		//draw rectangle, specify upr left and lwr right corner, pixel color (Black/White), Filled/NotFilled


// Timer Stuff
//----------------------------------------------
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
//---------------------------------------------------

void ADC_init(unsigned char usage) {
	//if(usage == 1){ // Joystick
		//ADMUX |= (1 << REFS0);
		//ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	//}
	//else{ // IR
		ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//}
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

uint16_t readadc(uint8_t ch)
{
	ch&=0b00000111;         //ANDing to limit input to 7
	ADMUX = (ADMUX & 0xf8)|ch;  //Clear last 3 bits of ADMUX, OR with ch
	ADCSRA|=(1<<ADSC);        //START CONVERSION
	while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE
	return(ADC);        //RETURN ADC VALUE
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
	unsigned char tempval = ~PINB & 0x03;
	if(tempval == 1){
		//LCD_DisplayString(1, "Updating..");
		unsigned short i = 0;
		for(i = 0; i< 489; i++){
			LCDBuffer[i] = LCDBuffer[i] + 1;
		}
		LCD_Update();
	}
	else if(tempval == 2){
		//LCD_DisplayString(1, "Updating..");
		unsigned short i = 0;
		for(i = 0; i< 489; i++){
			LCDBuffer[i] = LCDBuffer[i] - 1;
		}
		LCD_Update();
	}
	/*ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	short ud = readadc(0);
	PORTB = ud & 0xFF;
	static unsigned char pos = 17;
	unsigned char dOWN = ~PINA & 0x10;
	if(dOWN){
		
		LCD_Cursor(pos);
		LCD_WriteData(' ');
		LCD_Cursor(pos - 16);
		LCD_WriteData(0x0);
	}*/
}

void joyStick_Tick2(){
	static unsigned char pos = 17;
	short ud = readadc(4) - 512;
	PORTB = ud & 0xFF;
	unsigned char lr = ~PINC & 0x20;
	switch(JS_STATE){ //transitions
		case JS_init:
			if(ud > 0){
				JS_STATE = up;
			}
			else if(ud < 0){
				JS_STATE = up;
			}
			else{
				JS_STATE = JS_init;
			}
			break;
		case up:
			JS_STATE = up;
			break;
		case down:
			JS_STATE = maintainD;
			break;
		case maintainD:
			if(ud == 0){
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
			PORTC = 0x04;
			LCD_Cursor(pos);
			LCD_WriteData(' ');
			if(pos - 16 > 0){
				pos = pos - 16;
			}
			LCD_Cursor(pos);
			LCD_WriteData(0x0);
			break;
		case down:
			PORTC = 0x08;
			if(pos + 16 < 33){
				pos = pos + 16;
			}
			LCD_Cursor(pos);
			LCD_WriteData(0x0);
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
				return;
			}
		}
		pulses[currentPulse][0] = highPulse;
		while(x == y){ // the signal has gone back to being low (or how it was orriginally before button press)
			lowPulse++; // count how many iteratinos it is low for
			y = ADC;
			if(lowPulse >= MAX_VAL){
				return;
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
	static unsigned char powerBool = 0;
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

	if(averageHigh > 45 && averageHigh < 57 ){
		LCD_DisplayString(1, "Power!");
		LCD_Cursor(17);
		LCD_WriteData(0x0);
		if(powerBool == 0){
			unsigned char temp = PORTC & 0x80;
			if(temp){
				PORTC = PORTC & 0x7F;
				on = 0;
				LCD_clear();
				LCD_Update();
				LCD_DisplayString(1, "Goodbye");
				LCD_Cursor(17);
				LCD_WriteData(0x0);
				_delay_ms(10000);
				LCD_ClearScreen();
				powerBool = 0;
				return;
			}
			else{
				PORTC = PORTC | 0x80;
				on = 1;
				if(currChannel == 0){
					ReplaceBitmap(RobotHead);
					LCD_Update();
					_delay_ms(2500);
				}
				else if(currChannel == 1){
					ReplaceBitmap(WB);
					LCD_Update();
					_delay_ms(2500);
				}
				else{
					LCD_clear();
					DrawRectangle(5,5,79,40,Black, NotFilled);
					LCD_Update();
					_delay_ms(2500);
				}
			}
			powerBool = 1;
		}
	}
	else if(averageHigh > 77 && averageHigh < 84 && on){
		LCD_DisplayString(1, "Pointed Towards \"Channel UP\"");
		powerBool = 0;
		if(currChannel < 2){
			currChannel++;
		}
		
		if(currChannel == 0){
			ReplaceBitmap(RobotHead);
			LCD_Update();
			_delay_ms(2500);
		}
		else if(currChannel == 1){
			ReplaceBitmap(WB);
			LCD_Update();
			_delay_ms(2500);
		}
		else{
			LCD_clear();
			DrawRectangle(5,5,79,40,Black, NotFilled);
			LCD_Update();
			_delay_ms(2500);
		}
	}
	else if(averageHigh > 91 && averageHigh < 130 && on){
		LCD_DisplayString(1, "Pointed Away    \"Channel DOWN\"");
		powerBool = 0;
		if(currChannel > 0){
			currChannel --;
		}
		
		if(currChannel == 0){
			ReplaceBitmap(RobotHead);
			LCD_Update();
			_delay_ms(2500);
		}
		else if(currChannel == 1){
			ReplaceBitmap(WB);
			LCD_Update();
			_delay_ms(2500);
		}
		else{
			LCD_clear();
			DrawRectangle(5,5,79,40,Black, NotFilled);
			LCD_Update();
			_delay_ms(2500);
		}
	}

	/*unsigned long averageLow = 0;
	for(i = 0; i < currentPulse; i++){
		averageLow += pulses[i][1];
	}
	averageLow = averageLow / currentPulse;
	LCD_Cursor(17);
	LCD_WriteData(averageLow + ' '); */
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF; //Input
	DDRB = 0x00; PORTB = 0xFF; //Ouptut
	DDRC = 0xFF; PORTC = 0x00; 
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init('0');
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
		
	unsigned long GCD = 1;
	TimerSet(GCD);
	TimerOn();

	//LCD_DisplayString(1, strings[0]);

	LCD_5110_init();
	//LCDInitialize();
	
	unsigned short x = ADC;
	unsigned short y = 0;
	unsigned char count2 = 0;
	unsigned char nineAndTen = 0;
	unsigned short high = 0;
	
	unsigned char val = 0;
	/*for(val = 0; val < 33; val++){
		PORTB = val;
	}*/
	//PORTB = 0x1D;
	unsigned char never = 0;

	PORTC = PORTC & 0x7F;
	
	while (1){
		if (never == 0){ //for some reason this series of writeDatas only work when in the while(1) loop
			LCD_WriteCommand(0x40);
			//	0x0,0xa,0xa,0xa,0x0,0x11,0xe,0x0
			LCD_WriteData(0x0);
			LCD_WriteData(0xA);
			LCD_WriteData(0xA);
			LCD_WriteData(0xA);
			LCD_WriteData(0x0);
			LCD_WriteData(0x11);
			LCD_WriteData(0xE);
			LCD_WriteData(0x0);
			LCD_WriteCommand(0x80);
			never = 1;
		}

		//SetPixel(2,2,Black);
		//DrawRectangle(5,5,79,40,Black, NotFilled);
		//ReplaceBitmap(RobotHead);
		//LCD_Update();
		
		if(on){
			//ADC_init(1);
			joyStick_Tick();
		}
		//ADC_init(0);
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

void LCDInitialize(void){
	//SPCR |=1<<SPE | 1<<MSTR;										// if spi is used, uncomment this section out
	//SPSR |=1<<SPI2X;
	LCD_DataDir |=1<<DIN | 1<<CLK | 1<<CE | 1<<DC | 1<< RST;		// set spi pins and data/command and reset pins to output
	LCD_Port |=1<<RST;												// set reset high -> required for start up of display (receives a cycle in lcdinitialize)
	LCD_Port |=1<<PINB4;
	//LCD_Port |= 1 << CLK;
	
	_delay_ms(15);
	LCD_Port &=~1<<RST;			//resetpin lcd low
	_delay_ms(64);
	LCD_Port |=1<<RST;			//resetpin lcd high
	LCD_Port &=~1<<CE;			//CE pin low
	
	SendCommand_LCD(0x21);		//set lcd control in additional command set (H=1)
	SendCommand_LCD(0xB1);		//set lcd VOP control (contrast)
	SendCommand_LCD(0x04);		//set lcd Temp coefficient
	SendCommand_LCD(0x14);		//set lcd bias mode
	SendCommand_LCD(0x20);		//set lcd control in basic mode set (H=0)
	SendCommand_LCD(0x0C);		//set display configuration control in normal mode (alternative: blank, all on, inverse video)
	//SendCommand_LCD(0x0D);	//set display configuration control in inverse mode
	LCD_clear();
}

void LCD_5110_init(void) {
	//Set pins to output
	LCD_DataDir |= (1<<DIN) | (1<<CLK) | (1<<CE) | (1<<DC) | (1<<RST);

	//Set Reset to high
	LCD_Port |= (1<<RST);
	//Set Clock to high
	LCD_Port |= (1<<CLK);
	
	//Pulse Reset after some delay
	_delay_ms(15);
	LCD_Port |= (~1<<RST);
	_delay_ms(64);
	LCD_Port |= (1<<RST);

	//Set Chip Enable pin to Low
	LCD_Port |= (~1<<CE);

	//set LCD control in additional command set (H=1)
	SendCommand_LCD(0x21);
	//set LCD VOP control (contrast)
	SendCommand_LCD(0xBE);
	//set LCD Temp coefficient
	SendCommand_LCD(0x06);
	//set LCD bias mode
	SendCommand_LCD(0x13);
	//set LCD control in basic mode set (H=0)
	SendCommand_LCD(0x20);
	//set display configuration control in normal mode
	SendCommand_LCD(0x0C);
}

void LCD_clear(void){
	/* First flush the RAM memory of the LCD */
	int i,j;
	for(i=0; i<8; i++)
	for(j=0; j<90; j++)
	SendData_LCD(0x00);
	LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
	
	/* Clear the LCDBuffer array to remove any existing bitmaps*/
	for (int i=0; i<504;i++){
		LCDBuffer[i] = 0x00;
		SendData_LCD(LCDBuffer[i]);
	}
}

void SendData_LCD (unsigned int DataOut)
{
	LCD_Port |=1<<DC;							//	data/command high -> data
	LCD_Port |=1<<CE;							//	set chip enable high (has to be high, than low)
	LCD_Port&=(~(1<<CE));						// set chip enable low
	
	/*SPDR = DataOut;							// if spi is used, uncomment this section out

	//  Wait until Tx register empty.
	while ( !(SPSR & 0x80) );
	*/
	unsigned char i = 0;
	for(i=0; i<8; i++){					// if spi is used, comment this section out
		if((DataOut >> (7 - i)) & 0x01){
			LCD_Port |=1<<DIN; //lcd pin high
		} 
		else {
			LCD_Port&=(~(1<<DIN)); //lcd pin low
		}
		LCD_Port |=1<<CLK;			//Pulse the Clock line (Shift bit into register)
		LCD_Port&=(~(1<<CLK));
	}
	LCD_Port |=1<<CE;							// set chip enable high again
}

void SendCommand_LCD (unsigned int CommandOut){		// Can also be used for reading the SPI register which gets filled automatically as bits are shifted out
	LCD_Port |= (1<<DC);
	LCD_Port |= (1<<CE);							//	set chip enable high (has to be high, than low)
	LCD_Port &= (~1 << CE);							// set chip enable low
	LCD_Port &= (~1 << DC);							//	data/command low -> command	
	
	/* SPDR = CommandOut;						// if spi is used, uncomment this section out

	 //  Wait until Tx register empty.
	 while ( !(SPSR & 0x80) );
	 */
	 unsigned char i = 0;
	 for(i = 0; i < 8; i++){					// if spi is used, comment this section out
		if((CommandOut >> (7 - i)) & 0x01) {
			LCD_Port |= (1<<DIN); //lcd pin high
		} 
		else{
			LCD_Port &= (~1<<DIN); //lcd pin low
		}
		LCD_Port |= (1<<CLK);			//Pulse the Clock line (Shift bit into register)
		LCD_Port &= (~1 << CLK);	
	 }	
	LCD_Port |= (1 << CE);							// set chip enable high again
}

void LCD_gotoXY (unsigned char x, unsigned char y){
    SendCommand_LCD(0x80 | x);   //column
	SendCommand_LCD(0x40 | y);   //row
}

void LCD_Update (void){
	for (short i = 0; i < 504; i++){
		SendData_LCD(LCDBuffer[i]);
	}
}

void ReplaceBitmap (char ReplacementBitmap[]){	//replaces the bitmap by filling the LCDBuffer with the ReplacementBitmap bytes, LCD_Update required after this function
	for (short i = 0; i < 504; i++){
		LCDBuffer[i] = ReplacementBitmap[i];
	}
}

void SetPixel (unsigned char xp, unsigned char yp, char bw){
	int YBit = yp % 8;				//determine which bit is affected by using modulo
	int YByte = ((yp / 8) * 84) + xp;		//determine which byte is affected
	
	if (bw == Black){				//draw a black pixel
		LCDBuffer[YByte] |= (1<<YBit);
	}
	else{
		LCDBuffer[YByte] &= (~(1<<YBit));	//draw a white pixel
	}
}

void DrawRectangle (int x1,int y1, int x2, int y2, char bw, char fill)	// LCD_Update required after this function
{
	if (x1<0)	x1=0;	if (x1>83)	x1=83;
	if (x2<0)	x2=0;	if (x2>83)	x2=83;
	if (y1<0)	y1=0;	if (y1>47)	y1=47;
	if (y2<0)	y2=0;	if (y2>47)	y2=47;
	
	/*int RectangleStartByte = ((y1 / 8)*84)+x1;		// determine start byte of upper line -> upper left corner
	int RectangleEndByte = ((y2 /8)*84)+x1;			// determine start byte of lower line -> lower left corner
	char Rectangle_Y1_Bit = y1%8;					// which bit is affected in start byte
	char Rectangle_Y2_Bit = y2%8;					// which bit is affected in end byte
	char Rectangle_X_Delta = x2-x1;					// what is delta of x coordinates
	
	
	for (int i=0; i<Rectangle_X_Delta; i++)			// both horizontal lines of rectangle
	{
		LCDBuffer[i+RectangleStartByte]|=1<<Rectangle_Y1_Bit;
		LCDBuffer[i+RectangleEndByte] |=1<<Rectangle_Y2_Bit;
	}
	
	for (int i=0; i<8-Rectangle_Y1_Bit; i++)			// complement start + end bytes of upper line for vertical line
	{
		LCDBuffer[RectangleStartByte]|=1<<(Rectangle_Y1_Bit+i);
		LCDBuffer[RectangleStartByte+Rectangle_X_Delta] |=1<<(Rectangle_Y1_Bit+i);
	}
	
	for (int i=0; i<=Rectangle_Y2_Bit; i++)			// complement start + end bytes of upper line for vertical line
	{
		LCDBuffer[RectangleEndByte]|=1<<(Rectangle_Y2_Bit-i);
		LCDBuffer[RectangleEndByte+Rectangle_X_Delta] |=1<<(Rectangle_Y2_Bit-i);
	}
	while (RectangleStartByte<RectangleEndByte-84)
	{
		RectangleStartByte +=84;
		LCDBuffer[RectangleStartByte]|=0xFF;
		LCDBuffer[RectangleStartByte+Rectangle_X_Delta]|=0xFF;
	}*/
	if (fill == NotFilled)				//draw a not filled rectangle
	{
		for (int i=0; i<(x2-x1); i++)
		{
			SetPixel(i+x1,y1,bw);		// upper line
			SetPixel(i+x1,y2,bw);		// lower line
		}
		
		for (int i=0; i<=(y2-y1); i++)
		{
			SetPixel(x1,i+y1,bw);		// left line
			SetPixel(x2,i+y1,bw);		// right line
		}
	}
	
	else								//draw a filled rectangle by drawing a vertical line from x1 posn to x2 posn
	{
		for (int j=0; j<(x2-x1); j++)
			{
				for (int i=0; i<=(y2-y1); i++)
				{
					SetPixel(j+x1,i+y1,bw);		
			
				}
			}
	}
	
		
}
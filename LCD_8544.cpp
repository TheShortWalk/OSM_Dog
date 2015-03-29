/*
* LCD_8544.cpp
*
* Created: 3/15/2015 10:24:15 PM
* Author: Pooter
*/

#define F_CPU 16000000

#include "LCD_8544.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "spi.h"



// default constructor
LCD_8544::LCD_8544()
{
} //LCD_8544

void LCD_8544::Begin(){
	SPI.begin();
	PCD8544_DDR |= PIN_DC | PIN_CE;
	//PCD8544_PORT &= ~PIN_RESET;
	//_delay_ms(1);
	//PORTB |= PIN_RESET;
	
	this->LcdSend(LCD_CMD, 0x21);  // LCD Extended Commands.
	this->LcdSend(LCD_CMD, 0xBf);  // Set LCD Vop (Contrast). //B1
	this->LcdSend(LCD_CMD, 0x04);  // Set Temp coefficent. //0x04
	this->LcdSend(LCD_CMD, 0x14);  // LCD bias mode 1:48. //0x13
	this->LcdSend(LCD_CMD, 0x0C);  // LCD in normal mode. 0x0d for inverse
	this->LcdSend(LCD_C, 0x20);
	this->LcdSend(LCD_C, 0x0C);
	
	setStyle(NONE);
	
}

void LCD_8544::setStyle(FontStyle style){
	switch(style){
		case NONE:
			activeStyle[UNDERLINE] = false;
			activeStyle[STRIKETHROUGH] = false;
			activeStyle[INVERT] = false;
			break;
			
		case UNDERLINE:
			activeStyle[UNDERLINE] = true;
			
		case STRIKETHROUGH:
			activeStyle[STRIKETHROUGH] = true;
			
		case INVERT:
			activeStyle[INVERT] = true;
			
		default:
			break;
	}
}

//writes a string to the screen
void LCD_8544::Write(char *characters){
	while(*characters){
		this->LcdCharacter(*characters++);
	}
}

//writes a signed int to the screen, including negative sign
void LCD_8544::Write(int number){
	char str[6];
	itoa(number, str, 10);
	this->Write(str);
}

//writes a custom 5x8 symbol to the screen
void LCD_8544::Write(const uint8_t *symbol){
	uint8_t buf[5];
	memcpy_P(buf, symbol, 5);
	this->LcdSend(LCD_D, applyStyle(0x00));
	for (uint16_t index = 0; index < 5; index++)
	{
		this->LcdSend(LCD_D, applyStyle(buf[index]));
	}
	this->LcdSend(LCD_D, applyStyle(0x00));
}

//erases the entire screen
void LCD_8544::Clear(){
	for(uint16_t index = 0; index < LCD_X * LCD_Y / 8; index++){
		this->LcdSend(LCD_D, 0x00);
	}
}

//fills the entire screen
void LCD_8544::Fill(){
	for(uint16_t index = 0; index < LCD_X * LCD_Y / 8; index++){
		this->LcdSend(LCD_D, 0xFF);
	}
}


//moves the cursor
void LCD_8544::gotoXY(uint16_t x, uint16_t y){
	this->LcdSend(0, 0x80 | x); //column
	this->LcdSend(0, 0x40 | y); //row
}

void LCD_8544::gotoAlignX(CursorAllignment position){
	uint16_t x;
	switch (position)
	{
		case LEFT:
			x = 0;
			break;
		case CENTER:
			x = (LCD_X / 2) - 6;
			break;
		case RIGHT:
			x = (LCD_X) - 6;
			break;
		default:
			x = 0;
			break;
	}
	this->LcdSend(0, 0x80 | x);
}

void LCD_8544::LcdCharacter(char character){
	uint8_t buf[5];
	memcpy_P(buf, ASCII[character - 0x20], 5);
	this->LcdSend(LCD_D, applyStyle(0x00));
	for(int index = 0; index < 5; index++)
	{
		LcdSend(LCD_D, applyStyle(buf[index]));
	}
	this->LcdSend(LCD_D, applyStyle(0x00));
}

uint8_t LCD_8544::applyStyle(uint8_t data){
	uint8_t temp_data = data;
	if (activeStyle[UNDERLINE]) temp_data |= 0x40;
	if (activeStyle[STRIKETHROUGH]) temp_data |= 0x08;
	if (activeStyle[INVERT]) temp_data = ~temp_data;
	return temp_data;
}

void LCD_8544::LcdSend(uint8_t dc, uint8_t data){
	if(dc) PCD8544_PORT |= PIN_DC;
	else PCD8544_PORT &= ~PIN_DC;
	PCD8544_PORT &= ~PIN_CE;
	SPI.transfer(data);
	PCD8544_PORT |= PIN_CE;
}

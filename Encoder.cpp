/* 
* Encoder.cpp
*
* Created: 3/31/2015 11:41:18 AM
* Author: Pooter
*/


#include "Encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>

Encoder HID_Dial;

int blah = 0;

// default constructor
Encoder::Encoder()
{
	this->count = 0;
} //Encoder

void Encoder::Begin(){
	DDRD &= ~(1<<PORTD0) & ~(1<<PORTD1);	//Configure as input
	
	EICRA = (EICRA & ~((1<<ISC00) | (1<<ISC01))) | (1<<ISC00);		//Configure to RISING edge
	EIMSK |= (1<<INT0);						//Enable INTO
}

inline void Encoder::ISR_Handler(){
	//if(digitalRead(ENC_A) ^ digitalRead(ENC_B)) count++; //A XOR B
	//else count--;
	if(PORTD & ~(1<<PORTD1)) count++;
	else count--;
}

ISR(INT0_vect){
	//HID_Dial.ISR_Handler();
	//if(PORTD & ~(1<<PORTD1)) HID_Dial.count++;
	//else HID_Dial.count--;
	blah++;
}

// default destructor

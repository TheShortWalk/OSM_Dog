/* 
* HID_Components.cpp
*
* Created: 3/31/2015 11:41:18 AM
* Author: Pooter
*/


#include "HID_Components.h"
#include <avr/io.h>
#include <avr/interrupt.h>

Encoder HID_Dial;
Button	HID_Button;
bool HID_Changed;

//________________________Encoder________________________
Encoder::Encoder()
{
	this->count = 0;
} //Encoder

void Encoder::Begin(){
	PIN_A_DDRn &= ~(1<<PIN_A_PORTnx);	//Configure as input
	PIN_A_PORTn &= ~(1<<PIN_A_PORTnx);	//Turn off pull-up resistors
	PIN_B_DDRn &= ~(1<<PIN_B_PORTnx);
	PIN_B_PORTn &= ~(1<<PIN_B_PORTnx);
	
	DDRC |= (1<<PORTC7);		//Configure LED as output
	PORTC |= (1<<PORTC7);		//Turn DISPLAY off
	
	cli();
	
	EICRA = (EICRA & ~((1<<ISC00) | (1<<ISC01))) | (MODE_0<<ISC00);	//Configure to RISING edge
	EIMSK |= (1<<INT0);						//Enable INTO
	
	sei(); //enable interrupts
}

inline void Encoder::ISR_Handler(){
	if(PIN_B_PINn & (1<<PIN_B_PINnx)) HID_Dial.count--;
	else HID_Dial.count++;
	//PORTC ^= (1<<PORTC7); //toggle pin 13 LED
	HID_Changed = true;
	HID_Dial.Changed = true;
}

//________________________Button________________________
Button::Button()
{
	this->state = RELEASE; //assume released #YOLO
}

void Button::Begin(){
	PIN_BTN_DDRn &= ~(1<<PIN_BTN_PORTnx);	//Configure as input
	PIN_BTN_PORTn &= ~(1<<PIN_BTN_PORTnx);	//Turn off pull-up resistor
	
	cli();
	
	EICRA = (EICRA & ~((1<<ISC10) | (1<<ISC11))) | (MODE_1<<ISC10);	//Configure to RISING edge
	EIMSK |= (1<<INT1);
	
	sei(); //enable interrupts
}

inline void Button::ISR_Handler(){
	if(PIN_BTN_PINn & (1<<PIN_BTN_PORTnx)){
		 HID_Button.state = PRESS;
		 PORTC |= (1<<PORTC7);
	}
	else{
		 HID_Button.state = RELEASE;
		 PORTC &= (1<<PORTC7);
	}
	//PORTC ^= (1<<PORTC7); //toggle pin 13 LED
	HID_Changed = true;
	HID_Button.Changed = true;
}

//________________________ISR's________________________

//Handles RISING edge on encoder pinA
ISR(INT0_vect){
	HID_Dial.ISR_Handler();
}

//Handles CHANGEING edge on button pin
ISR(INT1_vect){
	HID_Button.ISR_Handler();
}


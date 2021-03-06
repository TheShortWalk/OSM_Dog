/* 
* HID_Components.h
*
* Created: 3/31/2015 11:41:18 AM
* Author: Pooter
*/


#ifndef __HID_Components_H__
#define __HID_Components_H__

#include <avr/io.h>
#include <avr/interrupt.h>

//Encoder Pins
#define PIN_A_PINn		PIND
#define PIN_A_DDRn		DDRD
#define PIN_A_PORTn		PORTD
#define PIN_A_PORTnx	PORTD0

#define PIN_B_PINn		PINE
#define PIN_B_DDRn		DDRE
#define PIN_B_PORTn		PORTE
#define PIN_B_PORTnx	PORTE6
#define PIN_B_PINnx		PINE6

//Button Pin
#define PIN_BTN_PINn	PIND
#define PIN_BTN_DDRn	DDRD
#define PIN_BTN_PORTn	PORTD
#define PIN_BTN_PORTnx	PORTD1

//ISRx trigger modes
enum INTx_Modes {LOW=0, CHANGE=1, FALLING=2, RISING=3};
#define MODE_0			RISING	//rising for encoder
#define MODE_1			CHANGE	//changing for button

class Encoder
{
public:
	Encoder();
	void Begin();
	volatile int8_t count;
	
	bool Changed;

	inline void ISR_Handler();
private:
};

class Button
{
public:
	Button();
	void Begin();
	
	bool Changed;
	
	enum ButtonStates {PRESS, RELEASE};
	volatile ButtonStates state;
	
	inline void ISR_Handler();
private:	
};

extern Encoder HID_Dial;
extern Button HID_Button;
extern bool HID_Changed; //indicates a change in HID values

//Encoder 

#endif //__HID_Components_H__

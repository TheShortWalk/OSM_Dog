/* 
* Encoder.h
*
* Created: 3/31/2015 11:41:18 AM
* Author: Pooter
*/


#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <avr/io.h>
#include <avr/interrupt.h>

//#define PIN_A (1 << INT0)

void Encoder_Begin();

class Encoder
{
public:
	Encoder();
	void Begin();
	volatile int count;

	uint8_t PORT_bit;

	inline void ISR_Handler();
private:
};

extern Encoder HID_Dial;

//Encoder 

#endif //__ENCODER_H__

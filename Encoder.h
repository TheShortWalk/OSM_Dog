/* 
* Encoder.h
*
* Created: 3/31/2015 11:41:18 AM
* Author: Pooter
*/


#ifndef __ENCODER_H__
#define __ENCODER_H__


class Encoder
{
public:
	Encoder();
	void Begin();
	volatile int16_t count;

private:
	void ISR_Handler();
};

Encoder 

#endif //__ENCODER_H__

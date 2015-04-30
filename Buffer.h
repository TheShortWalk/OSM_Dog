/*/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.

Buffer.h stores/loads buffered data during runtime. It contains the methods for requesting step time from AxisMotion.h.
An instance of this class is created for each axis.
The ISR will pull data from this class when running in Video Mode. The buffer is not use for timelapse or animation.
*/

#include "AxisMotion.h"

#ifndef Buffer_H
#define Buffer_H

#define BUFFER_SIZE 32 //must be power of 2

class Buffer_obj {
public:
	typedef void (Buffer_obj::*BuffMembFn)();
	//Buffer_obj();
	void attachAxis(AxisMotion_obj *LinkedAxis);

	volatile uint8_t PullPos, FillPos;
	bool Enabled;
	bool Finished;
	//The atmega32_u4 TIMER1 is a 16bit timer. Which overflows every .0328 seconds with 8 prescale
	//A second 16bit variable is used to keep track of the overflows.
	//Max time with this additional 32bit counter is 35.8 minutes
	uint16_t TimerCompare[BUFFER_SIZE]; //timer ticks in 16bit counter
	uint16_t OverflowCompare[BUFFER_SIZE]; //number of 16bit counter overflows
	bool Direction[BUFFER_SIZE];

	void Next();
	void Fill();
	void Reset();
	
	void attatchPath();
	void attatchTransition();

	void PrintBuffer();

private:
	AxisMotion_obj *targetAxis;
	uint32_t previousStepTime;
	uint32_t currentStep;
	bool currentDirection;
	uint8_t currentMicrostep;
	uint32_t microstepDuration;
	uint32_t nextMicrostepTime;
	uint32_t nextStepTime;\
	
	uint8_t buffTarget;

	void BufferOne();
	void BufferOneTrans();
	void Load2Buffer(uint32_t stepTime);
	
	//void(*BufferNext)();
	void BufferNext();

};

#endif
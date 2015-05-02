/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.

Controller.h is the overall controller for the system. It contains the top level methods for starting/stopping a motion program.
*/

#include <avr/io.h>

#ifndef AxisController_H
#define AxisController_H
#include "AxisMotion.h"
#include "Buffer.h"

struct IO_pin_struct{
	uint8_t _bit;
	volatile uint8_t *_port;
};

struct stepper_DriverPins{
	IO_pin_struct stepPin;
	IO_pin_struct directionPin;
};

class AxisController_obj {
	public:
	AxisController_obj(volatile uint8_t *stepPort, uint8_t stepBit, volatile uint8_t *directionPort, uint8_t directionBit); //Link Axis motion data to the buffer object
	AxisMotion_obj Motion;
	Buffer_obj Buffer;
	
	uint32_t getCurrentPosition();
	
	//private:
	stepper_DriverPins motorPin;
	volatile int32_t currentPosition;
	volatile bool Running;
};

#endif

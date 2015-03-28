/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include <avr/io.h>
#include "AxisController.h"

AxisController_obj::AxisController_obj(volatile uint8_t *stepPort, uint8_t stepBit, volatile uint8_t *directionPort, uint8_t directionBit){//(stepPort, stepBit, dirPort, dirBit)
	this->Buffer.attachAxis(&Motion);
	this->currentPosition = 0;
	this->motorPin.stepPin._port = stepPort;
	this->motorPin.stepPin._bit = 1 << stepBit;
	this->motorPin.directionPin._port = directionPort;
	this->motorPin.directionPin._bit = 1 << directionBit;
	
	
}

uint32_t AxisController_obj::getCurrentPosition(){
	return currentPosition;
}

/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include "Buffer.h"

/*
Buffer_obj::Buffer_obj(){
}
*/

void Buffer_obj::attachAxis(AxisMotion_obj *LinkedAxis) {
	this->targetAxis = LinkedAxis;
}

void Buffer_obj::Next()
{
	if (!Finished && (FillPos != PullPos) ) BufferOne();
}

void Buffer_obj::Fill()
{
	BufferOne(); //initialize buffer with one value
	while (!Finished && (FillPos != PullPos) ) BufferOne();
}

void Buffer_obj::PrintBuffer(){
	//Serial.println("Printing Buffer");
	for (int i = 0; i < BUFFER_SIZE; i++){
		//Serial.println(OverflowCompare[i] << 16 + TimerCompare[i]);
	}
}

void Buffer_obj::Reset()
{
	for (int i = 0; i < BUFFER_SIZE; i++) {
		TimerCompare[i] = 0;
		Direction[i] = 0;
	}
	PullPos = 0;
	FillPos = 0;
	Finished = false;
	currentStep = 0;
	currentMicrostep = MICROSTEPS + 1;
	previousStepTime = 0;
}

void Buffer_obj::BufferOne()
{
	uint32_t toBuffer;

	//linear interpolation of microsteps
	if (currentMicrostep < MICROSTEPS) {
		nextMicrostepTime += microstepDuration;
		toBuffer = nextMicrostepTime;
		currentMicrostep++;
	}

	//last step of interpolation is the full step time
	else if (currentMicrostep == MICROSTEPS) {
		toBuffer = nextStepTime;
		currentMicrostep++;
		previousStepTime = nextStepTime;
	}

	//next full step
	else {
		currentStep++;
		if (currentStep > targetAxis->totalSteps) {
			Finished = true; //compare to total steps
			toBuffer = 0;
		}
		else {
			currentMicrostep = 1;
			nextStepTime = targetAxis->StepTime(currentStep); //get the absolute step time from the axis object
			microstepDuration = (nextStepTime - previousStepTime) / MICROSTEPS; // "/ MICROSTEPS" should optimize into ">> 4"
			nextMicrostepTime = previousStepTime + microstepDuration;

			currentDirection = targetAxis->StepDirection(currentStep);

			toBuffer = nextMicrostepTime;
			currentMicrostep++; //
		}
	}
	Load2Buffer(toBuffer);
}

void Buffer_obj::BufferTransition(){
		uint32_t toBuffer;

		//linear interpolation of microsteps
		if (currentMicrostep < MICROSTEPS) {
			nextMicrostepTime += microstepDuration;
			toBuffer = nextMicrostepTime;
			currentMicrostep++;
		}

		//last step of interpolation is the full step time
		else if (currentMicrostep == MICROSTEPS) {
			toBuffer = nextStepTime;
			currentMicrostep++;
			previousStepTime = nextStepTime;
		}

		//next full step
		else {
			currentStep++;
			if (currentStep > targetAxis->totalSteps) {
				Finished = true; //compare to total steps
				toBuffer = 0;
			}
			else {
				currentMicrostep = 1;
				nextStepTime = targetAxis->StepTime(currentStep); //get the absolute step time from the axis object
				microstepDuration = (nextStepTime - previousStepTime) / MICROSTEPS; // "/ MICROSTEPS" should optimize into ">> 4"
				nextMicrostepTime = previousStepTime + microstepDuration;

				currentDirection = targetAxis->StepDirection(currentStep);

				toBuffer = nextMicrostepTime;
				currentMicrostep++; //
			}
		}
		Load2Buffer(toBuffer);
}

void Buffer_obj::Load2Buffer(uint32_t stepTime)
{
	TimerCompare[FillPos] = stepTime & 0xFFFF;
	OverflowCompare[FillPos] = stepTime >> 16;
	Direction[FillPos] = currentDirection;

	FillPos++;
	if (FillPos >= BUFFER_SIZE) FillPos = 0;
}

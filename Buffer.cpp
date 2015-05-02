/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include "Buffer.h"
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

/*
Buffer_obj::Buffer_obj(){
}
*/

void Buffer_obj::attachAxis(AxisMotion_obj *LinkedAxis) {
	this->targetAxis = LinkedAxis;
}

void Buffer_obj::Next()
{
	if (!Finished && (FillPos != PullPos) ) BufferNext();
}

void Buffer_obj::Fill()
{
	uint8_t nextFillPos;
	do{
		 BufferNext();
		 nextFillPos = (FillPos + 1) & (BUFFER_SIZE - 1); //ring buffer
	} while (!Finished && (nextFillPos != PullPos) );
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

void Buffer_obj::attatchPath(){
	buffTarget = 0;
}

void Buffer_obj::attatchTransition(){
	buffTarget = 1;
}

//this is soooo shitty
void Buffer_obj::BufferNext(){
	if(buffTarget == 0) BufferOne();
	else if(buffTarget == 1) BufferOneTrans();
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

void Buffer_obj::BufferOneTrans()
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
		if (currentStep > targetAxis->totalStepsTrans) {
			Finished = true; //compare to total steps
			return;
			//toBuffer = 0;
		}
		else {
			currentMicrostep = 1;
			nextStepTime = targetAxis->transitionSegment.getStepTimer(currentStep); //get the absolute step time from the axis object
			microstepDuration = (nextStepTime - previousStepTime) / MICROSTEPS; // "/ MICROSTEPS" should optimize into ">> 4"
			nextMicrostepTime = previousStepTime + microstepDuration;

			currentDirection = targetAxis->transitionSegment.getDirection();

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

	FillPos = (FillPos + 1) & (BUFFER_SIZE - 1); //ring buffer
	//if (FillPos >= BUFFER_SIZE) FillPos = 0;
}

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
	//Serial.println("Buffer Next");
	if (!Finished && (FillPos != PullPos) ) BufferOne();
	//if (!Finished) BufferOne();
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
		//Serial.println("if");
		nextMicrostepTime += microstepDuration;
		//Load2Buffer(nextMicrostepTime);
		toBuffer = nextMicrostepTime;
		currentMicrostep++;
	}

	//last step of interpolation is the full step time
	else if (currentMicrostep == MICROSTEPS) {
		//Serial.println("else if");
		//Load2Buffer(nextStepTime);
		toBuffer = nextStepTime;
		currentMicrostep++;
		previousStepTime = nextStepTime;
	}

	//next full step
	else {
		//Serial.println("else");
		currentStep++;
		if (currentStep > targetAxis->totalSteps) {
			Finished = true; //compare to total steps
			//Load2Buffer(0); //set buffer to 0 to disable ISR when finished
			toBuffer = 0;
		}
		else {
			currentMicrostep = 1;
			nextStepTime = targetAxis->StepTime(currentStep); //get the absolute step time from the axis object
			microstepDuration = (nextStepTime - previousStepTime) / MICROSTEPS; // "/ MICROSTEPS" should optimize into ">> 4"
			nextMicrostepTime = previousStepTime + microstepDuration;

			//Serial.print("Step Time: "), Serial.println(nextStepTime);

			currentDirection = targetAxis->StepDirection(currentStep);
			//Serial.print("Direction: "), Serial.println(currentDirection);

			//Load2Buffer(nextMicrostepTime);
			toBuffer = nextMicrostepTime;
			currentMicrostep++; //


			//Serial.print("---FULL STEP: "), Serial.print(currentStep), Serial.println("-----");
			//Serial.print("TOTAL: "), Serial.println(nextStepTime);
			//Serial.print("Compare: "), Serial.println(TimerCompare[FillPos]);
			//Serial.print("Overflw: "), Serial.println(OverflowCompare[FillPos]);
			//Serial.print("Dir: "), Serial.println(currentDirection);
		}
	}
	Load2Buffer(toBuffer);
}

void Buffer_obj::Load2Buffer(uint32_t stepTime)
{
	TimerCompare[FillPos] = stepTime & 0xFFFF;
	OverflowCompare[FillPos] = stepTime >> 16;
	Direction[FillPos] = currentDirection;

	//  Serial.print("---MICROSTEP: "), Serial.print(currentMicrostep), Serial.println("-----");
	//  Serial.print("TOTAL: "), Serial.println(stepTime);
	//  Serial.print("Compare: "), Serial.println(TimerCompare[FillPos]);
	//  Serial.print("Overflw: "), Serial.println(OverflowCompare[FillPos]);
	//  Serial.print("Dir: "), Serial.println(currentDirection);

	FillPos++;
	if (FillPos >= BUFFER_SIZE) FillPos = 0;
}

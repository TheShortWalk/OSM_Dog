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

bool Buffer_obj::Next()
{
	//if the move has not cycled through the buffer
	if (!Finished){
		//ring buffer
		uint8_t nextFillPos =  (FillPos + 1) & (BUFFER_SIZE - 1);
		
		//the buffer is not full
		if(nextFillPos != PullPos){
			FillPos = nextFillPos;
			BufferNext();
			return true; //successful buffer
		}
		
		//the buffer is full
		else return false; //unsuccessful buffer
	}
	return false; //unsuccessful buffer
}


//implement a system to select a fill position, and indicate it's fill status
//currently the fill position has a 1 space gap to the pull position
//the ISR checks to see if it's pulling from the fill position to halt the move
//indicating an overrun, or the move is finished and has stopped buffering.
void Buffer_obj::Fill()
{
	Next();
	PullPos = 0;
	while (Next());
/*
	uint8_t nextFillPos;
	do{
		 Next();
		 nextFillPos = (FillPos + 1) & (BUFFER_SIZE - 1); //ring buffer
	} while (!Finished && (nextFillPos != PullPos) );
	*/
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
	PullPos = BUFFER_SIZE - 1;
	FillPos = BUFFER_SIZE - 1;
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

	//uint8_t nextFillPos = (FillPos + 1) & (BUFFER_SIZE - 1);
	//if(nextFillPos == PullPos) Full = true;
	//else Full = false;
}

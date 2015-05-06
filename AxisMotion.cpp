/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.
*/

#include <stdlib.h>
#include "AxisMotion.h"

AxisMotion_obj::AxisMotion_obj(){
	this->totalSegments = 0;
}

bool AxisMotion_obj::AddSegment(uint8_t segment_number) {
	if (segment_number > MAX_SEGMENTS) return false;

	if (totalSegments < MAX_SEGMENTS) {
		totalSegments++;
		//shift key data around if between keys
		if (segment_number == 0) Segment[segment_number].start = Segment[segment_number - 1].finish; //start of new segment is end of prev segment

		return true;
	}

	else return false; //unable to add keyframe
}

bool AxisMotion_obj::DeleteSegment(uint8_t segment_number) {
	if (totalSegments > 1) {
		totalSegments--;
		//shift key data around if between keys
		return true;
	}

	else return false; //unable to delete keyframe
}

uint32_t AxisMotion_obj::StepTime(uint32_t StepNumber) {
	uint8_t i = getSegment(StepNumber);
	uint32_t relativeStep;
	if(i == 0) relativeStep = StepNumber;
	else relativeStep = StepNumber - totalSegmentSteps[i - 1];
	return Segment[i].getStepTimer(relativeStep);
}

bool AxisMotion_obj::StepDirection(uint32_t StepNumber) {
	uint8_t seg = getSegment(StepNumber);
	return Segment[seg].getDirection();
}

bool AxisMotion_obj::StepDirection(float time_seconds){
	uint8_t seg = getSegment(time_seconds);
	return Segment[seg].getDirection();
}

void AxisMotion_obj::CalculateMove() {
	for (uint8_t i = 0; i < totalSegments; i++) {
		Segment[i].CalcSegment();
	}
	transitionSegment.CalcSegment();
	CalculateTotals();
}

void AxisMotion_obj::PrintMove() {
	for (uint8_t i = 0; i < totalSegments; i++) {
		//Serial.print("------- Segment "), Serial.print(i), Serial.println("-------");
		//Segment[i].PrintSegment();
	}
}

int32_t AxisMotion_obj::getStep_AtTime(float time_seconds){
	//return step position at given time
	uint8_t seg = getSegment(time_seconds);
	float startTime = Segment[seg].start.seconds;
	float relativeTime = time_seconds - startTime;
	return Segment[seg].getStep(relativeTime);
}

//-----PRIVATE FUNCTIONS----------

void AxisMotion_obj::CalculateTotals(){
	uint32_t total = 0;
	for(uint8_t i = 0; i < totalSegments; i++){
		total += abs(Segment[i].finish.steps - Segment[i].start.steps);
		totalSegmentSteps[i] = total;
	}
	totalSteps = total;
	totalSeconds = Segment[totalSegments - 1].finish.seconds;
	totalStepsTrans = abs(transitionSegment.finish.steps - transitionSegment.start.steps);
}

uint8_t AxisMotion_obj::getSegment(uint32_t StepNumber){
	//uint8_t segmentNumber;
	for(uint8_t i = 0; i < totalSegments; i++){
		if(StepNumber <= totalSegmentSteps[i]){
			return i;
		}
	}
	//end of move
	return 0;
}

uint8_t AxisMotion_obj::getSegment(float time_seconds){
	for (uint8_t i = 0; i < totalSegments; i++){
		if (time_seconds < Segment[i].finish.seconds)
		{
			return i;
		}
	}
	//end of move
	return totalSegments - 1;
}

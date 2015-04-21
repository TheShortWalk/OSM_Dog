/*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.

AxisMotion.h describes the motion of a single axis. It contains methods for reading/writing the motion program.
The motion of an axis consists of a series of segments(s-curve).
*/

#ifndef AxisMotion_H
#define AxisMotion_H
#include "S_Curve.h"


#define MAX_SEGMENTS 4

class AxisMotion_obj {
	public:
	AxisMotion_obj();
	Segment_obj Segment[MAX_SEGMENTS];
	Segment_obj gotoSegment;

	uint32_t totalSteps; //absolute step of whole move
	uint32_t totalSegmentSteps[MAX_SEGMENTS]; //absolute step of each segment
	float totalSeconds;
	uint8_t totalSegments;
	//bool Enabled = false;

	bool AddSegment(uint8_t segment_number);
	bool DeleteSegment(uint8_t segment_number);
	uint32_t StepTime(uint32_t StepNumber); //absolute time in counter ticks
	bool StepDirection(uint32_t StepNumber);
	void CalculateMove();
	int32_t getStep_AtTime(float time_seconds);
	void PrintMove();
	
	private:
	void CalculateTotals();
	uint8_t getSegment(uint32_t StepNumber);
	uint8_t getSegment(float time_seconds);
};

#endif
 /*
Copyright (C) 2014 William Osman <theshortwalk@gmail.com>

Insert Legal crap here.

S_Curve.h is the ramping function for the controller. It contains methods for reading/writing each segment.
It also contains an optimized algorithm for high-speed runtime calculations.
Each s-curve segment contains start/end points and smooting value to adjust the ramp.
*/

#include <avr/io.h>

#ifndef S_Curve_H
#define S_Curve_H

#define MICROSTEPS 16 //need to update FastMath() if this is modified

class Point_obj {
  public:
    int32_t steps;
    float seconds;

    Point_obj();
    void offset(int16_t offsetSteps, float offsetSeconds);
    void set(int32_t setSteps, float setSeconds);
    void PrintPoint();
};

//************************************************************
class Segment_obj {
  public:
    Point_obj start, finish, midPoint_A, midPoint_B;
    float acceleration, velocity;
    uint32_t accelFactor_1, accelFactor_2, velFactor, start_ticks, deltaSteps, midPoint_A_ticks, deltaTime_ticks;
    
    uint8_t smoothing;
	float deltaTime;
	
    void offset(int16_t offsetSteps, float offsetSeconds);
    void CalcSegment();
    uint32_t getStepTimer(uint32_t Step);
	int32_t getStep(float time_seconds);
	bool getDirection();
    void PrintSegment();

  private:
	bool Direction;
    void PreCalc(); //Computes S-Curve for runtime
    uint8_t GetSubSegment(uint32_t nextStep);
	uint8_t GetSubSegment(float time_seconds);

};

#endif

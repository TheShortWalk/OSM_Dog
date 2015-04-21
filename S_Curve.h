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
    void offset(int offsetSteps, float offsetSeconds);
    void set(long setSteps, float setSeconds);
    void PrintPoint();
};

//************************************************************
class Segment_obj {
  public:
    Point_obj start, finish, midPoint_A, midPoint_B;
    float acceleration, velocity;
    int32_t accelFactor_1, accelFactor_2, velFactor;
    bool Direction;
    uint8_t smoothing;
    uint32_t deltaSteps;

    void offset(int offsetSteps, float offsetSeconds);
    void CalcSegment();
    uint32_t getStepTimer(uint32_t Step);
	int32_t getStep(float time_seconds);
    void PrintSegment();

  private:
    void PreCalc(); //Computes S-Curve for runtime
    uint8_t GetSubSegment(uint32_t nextStep);
	uint8_t GetSubSegment(float time_seconds);

};

#endif
